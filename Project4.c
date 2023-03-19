#include <iostream>
#include <fstream>
#include <cstdlib>

using namespace std;

class Property {
public:
    int label;
    int numPixels;
    int minR;
    int minC;
    int maxR;
    int maxC;

    Property(int _label = 0, int _numPixels = 0, int _minR = 0, int _minC = 0, int _maxR = 0, int _maxC = 0) {
        label = _label;
        numPixels = _numPixels;
        minR = _minR;
        minC = _minC;
        maxR = _maxR;
        maxC = _maxC;
    }
};

class ccLabel {
public:
    int numRows;
    int numCols;
    int minVal;
    int maxVal;
    int newLabel;
    int trueNumCC;
    int newMin;
    int newMax;
    int **zeroFramedAry;
    int *NonZeroNeighborAry;
    int *EQAry;
    char option;
    Property *CCproperty;

    // Constructor
    ccLabel(int rows, int cols, int min, int max) {
        numRows = rows;
        numCols = cols;
        minVal = min;
        maxVal = max;
        newLabel = 0;
        trueNumCC = 0;
        newMin = 0;
        newMax = 0;

        // Dynamically allocate zeroFramedAry
        zeroFramedAry = new int*[numRows+2];
        for (int i = 0; i < numRows+2; i++) {
            zeroFramedAry[i] = new int[numCols+2];
            for (int j = 0; j < numCols+2; j++) {
                zeroFramedAry[i][j] = 0;
            }
        }

        // Initialize EQAry
        int size = (numRows * numCols) / 4;
        EQAry = new int[size];
        negative1D(EQAry, size);

        // Dynamically allocate CCproperty
        CCproperty = new Property[size];
    }

    // Destructor
    ~ccLabel() {
        for (int i = 0; i < numRows+2; i++) {
            delete[] zeroFramedAry[i];
        }
        delete[] zeroFramedAry;
        delete[] EQAry;
        delete[] CCproperty;
    }

    void loadImg(ifstream& imgFile, int** zeroFramedAry) {
        for (int i = 1; i < numRows+1 ; i++) {
            for (int j = 1; j < numCols+1 ; j++) {
                imgFile >> zeroFramedAry[i][j];
            }
        }
    }

    void zero2D(int** ary) {
        for (int i = 0; i < numRows + 2; i++) {
            for (int j = 0; j < numCols + 2; j++) {
                ary[i][j] = 0;
            }
        }
    }

    void negative1D(int* ary, int size) {
        for (int i = 0; i < size; i++) {
            ary[i] = -1;
        }
    }



    void conversion() {
        for (int i = 1; i < numRows + 1; i++) {
            for (int j = 1; j < numCols + 1; j++) {
                if (zeroFramedAry[i][j] == 0) {
                    zeroFramedAry[i][j] = 1;
                }
                else {
                    zeroFramedAry[i][j] = 0;
                }
            }
        }
    }
    void imgReformat(int **zeroFramedAry, ofstream& outFile) {
        outFile << "Rows " << numRows << " Cols " << numCols << " minVal " << minVal << " maxVal " << maxVal << endl;
        string str = to_string(maxVal);
        int Width = str.length();
        for (int r = 0; r < numRows ; r++) {
            for (int c = 0; c < numCols ; c++) {
            outFile << zeroFramedAry[r][c] << " ";
            str = to_string(zeroFramedAry[r][c]);
            int WW = str.length();
                while (WW < Width) {
                outFile << " ";
                WW++;
                }
        }
            outFile << endl;
        }
}
void connect8Pass1(int** zeroFramedAry, int newLabel, int* EQAry, int numRows, int numCols){
    // Step 0: Initialize the equivalency table
    int nextLabel = 1;
    for(int i=0; i<numRows*numCols; i++){
        EQAry[i] = i;
    }
    
    // Step 1: Scan the image left to right, top to bottom
    for(int i=1; i<=numRows; i++){
        for(int j=1; j<=numCols; j++){
            if(zeroFramedAry[i][j] > 0){
                // Step 2: Look at the pixels a through h
                int a = zeroFramedAry[i-1][j-1];
                int b = zeroFramedAry[i-1][j];
                int c = zeroFramedAry[i-1][j+1];
                int d = zeroFramedAry[i][j-1];
                
                // Case 1: a, b, c, and d are all 0
                if(a==0 && b==0 && c==0 && d==0){
                    zeroFramedAry[i][j] = nextLabel;
                    nextLabel++;
                }
                // Case 2: a=b=c=d=p(i,j)
                else if(a==b && b==c && c==d && d==zeroFramedAry[i][j]){
                    zeroFramedAry[i][j] = a;
                }
                // Case 3: At least two different labels
                else{
                    int minLabel = zeroFramedAry[i][j];
                    if(a > 0 && a < minLabel) minLabel = a;
                    if(b > 0 && b < minLabel) minLabel = b;
                    if(c > 0 && c < minLabel) minLabel = c;
                    if(d > 0 && d < minLabel) minLabel = d;
                    if(zeroFramedAry[i][j] > minLabel){
                        EQAry[zeroFramedAry[i][j]] = minLabel;
                        zeroFramedAry[i][j] = minLabel;
                    }
                    else{
                        EQAry[minLabel] = zeroFramedAry[i][j];
                        zeroFramedAry[i][j] = minLabel;
                    }
                }
            }
        }
    }
}


void connect8Pass2(int** zeroFramedAry, int numRows, int numCols, int* newLabel, int** EQAry) {
    // Scan the result of Pass 1 right to left, bottom to top
    for (int i = numRows; i > 0; i--) {
        for (int j = numCols; j > 0; j--) {
            if (zeroFramedAry[i][j] > 0) {
                int a = zeroFramedAry[i - 1][j - 1];
                int b = zeroFramedAry[i - 1][j];
                int c = zeroFramedAry[i - 1][j + 1];
                int d = zeroFramedAry[i][j - 1];
                int e = zeroFramedAry[i][j + 1];
                int f = zeroFramedAry[i + 1][j - 1];
                int g = zeroFramedAry[i + 1][j];
                int h = zeroFramedAry[i + 1][j + 1];

                // Case 1: e=f=g=h=0
                if (e == 0 && f == 0 && g == 0 && h == 0) {
                    // Do nothing, p(i,j) keeps its label
                }
                // Case 2: e=f=g=h=p(i,j)
                else if (e == zeroFramedAry[i][j] && f == zeroFramedAry[i][j] && g == zeroFramedAry[i][j] && h == zeroFramedAry[i][j]) {
                    // Do nothing, p(i,j) keeps its label
                }
                // Case 3: At least 2 different labels
                else {
                    int minLabel = min(a, min(b, min(c, min(d, min(e, min(f, min(g, h))))));
                    if (zeroFramedAry[i][j] > minLabel) {
                        EQAry[zeroFramedAry[i][j]][0] = minLabel;
                        zeroFramedAry[i][j] = newLabel[minLabel];
                    }
                }
            }
        }
    }

    // Use the equivalency table to update pixels that were not updated in step 2
    for (int i = 1; i <= numRows; i++) {
        for (int j = 1; j <= numCols; j++) {
            if (zeroFramedAry[i][j] > 0) {
                zeroFramedAry[i][j] = newLabel[EQAry[zeroFramedAry[i][j]][0]];
            }
        }
    }
}

    // 4-connected Pass1
void Connect4Pass1(int** zeroFramedAry, int& newLabel, int* EQAry, int numRows, int numCols) {
    newLabel = 0;
    int minLabel = 0;
    for (int i = 1; i <= numRows; i++) {
        for (int j = 1; j <= numCols; j++) {
            int pixel = zeroFramedAry[i][j];
            if (pixel > 0) {
                int a = zeroFramedAry[i-1][j-1];
                int b = zeroFramedAry[i-1][j];
                int c = zeroFramedAry[i-1][j+1];
                int d = zeroFramedAry[i][j-1];
                if (a == 0 && b == 0) {
                    newLabel++;
                    zeroFramedAry[i][j] = newLabel;
                }
                else if (a == b) {
                    zeroFramedAry[i][j] = a;
                }
                else {
                    minLabel = min(a, b);
                    zeroFramedAry[i][j] = minLabel;
                    EQAry[max(a, b)] = minLabel;
                }
            }
        }
    }
}



void connect4Pass2(int** zeroFramedAry, int newLabel, int* EQAry) {
    // Scan the result of pass 1, right to left, bottom to top
    for (int i = numRows + 1; i >= 1; i--) {
        for (int j = numCols + 1; j >= 1; j--) {
            int p = zeroFramedAry[i][j];
            // If p is not 0
            if (p > 0) {
                int c = zeroFramedAry[i][j + 1];
                int d = zeroFramedAry[i + 1][j + 1];
                // Look at c and d
                if (c == 0 && d == 0) {
                    // Do nothing, p keeps its label
                } else if (c == d && d == p) {
                    // Do nothing, p keeps its label
                } else {
                    int minLabel = p;
                    if (c > 0 && c < minLabel) {
                        minLabel = c;
                    }
                    if (d > 0 && d < minLabel) {
                        minLabel = d;
                    }
                    // Update p with the minimum label
                    zeroFramedAry[i][j] = minLabel;
                    // If p was not already updated, update the equivalency table
                    if (p > minLabel) {
                        EQAry[p] = minLabel;
                    }
                }
            }
        }
    }
    // Use the equivalency table to update any remaining pixels
    for (int i = 1; i <= numRows + 1; i++) {
        for (int j = 1; j <= numCols + 1; j++) {
            int p = zeroFramedAry[i][j];
            if (p > 0 && EQAry[p] > 0) {
                zeroFramedAry[i][j] = EQAry[p];
            }
        }
    }
}


// 3-pass algorithm to label 8-connected components
void connectPass3(int** zeroFramedAry, int* EQAry, CCProperty* CCproperty, int trueNumCC, ofstream& deBugFile) {
    deBugFile << "entering connectPass3 method" << endl;
    for (int i = 1; i <= trueNumCC; i++) {
        CCproperty[i].label = i;
        CCproperty[i].numPixels = 0;
        CCproperty[i].minR = numRow;
        CCproperty[i].maxR = 0;
        CCproperty[i].minC = numCol;
        CCproperty[i].maxC = 0;
    }
    for (int r = 1; r < numRow + 1; r++) {
        for (int c = 1; c < numCol + 1; c++) {
            int p = zeroFramedAry[r][c];
            if (p > 0) {
                zeroFramedAry[r][c] = EQAry[p];
                int k = zeroFramedAry[r][c];
                CCproperty[k].numPixels++;
                if (r < CCproperty[k].minR)
                    CCproperty[k].minR = r;
                if (r > CCproperty[k].maxR)
                    CCproperty[k].maxR = r;
                if (c < CCproperty[k].minC)
                    CCproperty[k].minC = c;
                if (c > CCproperty[k].maxC)
                    CCproperty[k].maxC = c;
            }
        }
    }
    deBugFile << "leaving connectPass3 method" << endl;
}


int manageEQAry(int* EQAry, int newLabel) {
    int count = 0;
    for (int i = 1; i <= newLabel; i++) {
        if (EQAry[i] == i) {
            count++;
        } else {
            EQAry[i] = EQAry[EQAry[i]];
        }
    }
    return count;
}
void printCCproperty(CCproperty* CCprop, int trueNumCC, const string& propertyFile) {
    ofstream outFile(propertyFile);
    if (!outFile.is_open()) {
        cerr << "Error: Unable to open property file." << endl;
        return;
    }
    outFile << "The number of connected components found: " << trueNumCC << endl;
    for (int i = 1; i <= trueNumCC; i++) {
        outFile << "-------------------------" << endl;
        outFile << "Property of CC " << i << endl;
        outFile << "-------------------------" << endl;
        outFile << "Label: " << CCprop[i].label << endl;
        outFile << "Number of pixels: " << CCprop[i].numPixels << endl;
        outFile << "Bounding box: (" << CCprop[i].minR << "," << CCprop[i].minC << ") - (" << CCprop[i].maxR << "," << CCprop[i].maxC << ")" << endl;
    }
    outFile.close();
    cout << "Component properties have been successfully written to " << propertyFile << endl;
}


void printEQAry(int* EQAry, int newLabel) {
    cout << "EQAry:" << endl;
    for (int i = 1; i <= newLabel; i++) {
        cout << EQAry[i] << " ";
    }
    cout << endl;
}

void drawBoxes(int** zeroFramedAry, Property* CCproperty, int trueNumCC) {
    int index, minRow, minCol, maxRow, maxCol, label;

    // Loop through each connected component property
    for (index = 1; index <= trueNumCC; index++) {
        // Get the minimum and maximum row and column values, and the label
        minRow = CCproperty[index].minR + 1;
        minCol = CCproperty[index].minC + 1;
        maxRow = CCproperty[index].maxR + 1;
        maxCol = CCproperty[index].maxC + 1;
        label = CCproperty[index].label;

        // Draw the label on the minimum and maximum rows and columns
        for (int i = minCol; i <= maxCol; i++) {
            zeroFramedAry[minRow][i] = label;
            zeroFramedAry[maxRow][i] = label;
        }
        for (int i = minRow; i <= maxRow; i++) {
            zeroFramedAry[i][minCol] = label;
            zeroFramedAry[i][maxCol] = label;
        }
    }
}


void printImg(int** zeroFramedAry, int numRows, int numCols, string outputFile) {
    ofstream out(outputFile);
    out << numRows << " " << numCols << " " << 255 << endl;
    for (int i = 1; i <= numRows; i++) {
        for (int j = 1; j <= numCols; j++) {
            out << zeroFramedAry[i][j] << " ";
        }
        out << endl;
    }
    out.close();
}
int main(int argc, char* argv[]) {
    ifstream inFile(argv[1]);
    int connectivity = stoi(argv[2]);
    char option = argv[3][0];
    ofstream RFprettyPrintFile(argv[4]);
    ofstream labelFile(argv[5]);
    ofstream propertyFile(argv[6]);
    ofstream deBugFile(argv[7]);
    int numRows, numCols, minVal, maxVal;
    inFile >> numRows >> numCols >> minVal >> maxVal;
    int** zeroFramedAry = new int*[numRows+2];
    for (int i = 0; i < numRows+2; i++) {
        zeroFramedAry[i] = new int[numCols+2];
    }
    int newLabel = 0;
    zero2D(zeroFramedAry, numRows+2, numCols+2);
    loadImage(inFile, zeroFramedAry, numRows, numCols);

    if (option == 'y' || option == 'Y') {
        conversion(zeroFramedAry, numRows, numCols);
    }

    int** EQAry = new int*[numRows+2];
    for (int i = 0; i < numRows+2; i++) {
        EQAry[i] = new int[numCols+2];
    }

    if (connectivity == 4) {
        connected4(zeroFramedAry, newLabel, EQAry, RFprettyPrintFile, deBugFile);
    }
    else if (connectivity == 8) {
        connected8(zeroFramedAry, newLabel, EQAry, RFprettyPrintFile, deBugFile);
    }

    labelFile << numRows << " " << numCols << " " << newMin << " " << newMax << endl;
    printImg(zeroFramedAry, labelFile, numRows, numCols);
    int trueNumCC = manageEQAry(EQAry, newLabel);
    printCCproperty(propertyFile, CCproperty, trueNumCC);
    drawBoxes(zeroFramedAry, CCproperty, trueNumCC);
    imgReformat(zeroFramedAry, RFprettyPrintFile, numRows, numCols);
    RFprettyPrintFile << "Number of Connected Components: " << trueNumCC << endl;

    for (int i = 0; i < numRows+2; i++) {
        delete[] zeroFramedAry[i];
        delete[] EQAry[i];
    }
    delete[] zeroFramedAry;
    delete[] EQAry;

    inFile.close();
    RFprettyPrintFile.close();
    labelFile.close();
    propertyFile.close();
    deBugFile.close();

    return 0;
}
void connected4(int** zeroFramedAry, int& newLabel, int* EQAry, ofstream& RFprettyPrintFile, ofstream& deBugFile) {
    // Step 0
    deBugFile << "entering connected4 method" << endl;
    
    // Step 1
    connect4Pass1(zeroFramedAry, newLabel, EQAry);
    deBugFile << "After connected4 pass1, newLabel = " << newLabel << endl;
    imgReformat(zeroFramedAry, RFprettyPrintFile);
    printEQAry(newLabel, EQAry, RFprettyPrintFile);
    
    // Step 2
    Connect4Pass2(zeroFramedAry, EQAry);
    deBugFile << "After connected4 pass2, newLabel = " << newLabel << endl;
    imgReformat(zeroFramedAry, RFprettyPrintFile);
    printEQAry(newLabel, EQAry, RFprettyPrintFile);
    
    // Step 3
    int trueNumCC = manageEQAry(EQAry, newLabel);
    printEQAry(newLabel, EQAry, RFprettyPrintFile);
    int newMin = 0;
    int newMax = trueNumCC;
    int* CCproperty = new int[trueNumCC+1];
    deBugFile << "In connected4, after manage EQAry, trueNumCC = " << trueNumCC << endl;
    
    // Step 4
    connectPass3(zeroFramedAry, EQAry, CCproperty, trueNumCC, deBugFile);
    
    // Step 5
    imgReformat(zeroFramedAry, RFprettyPrintFile);
    
    // Step 6
    printEQAry(newLabel, EQAry, RFprettyPrintFile);
    
    // Step 7
    deBugFile << "Leaving connected4 method" << endl;
}
void connected8(int** zeroFramedAry, int* newLabel, int* EQAry, ofstream& RFprettyPrintFile, ofstream& deBugFile) {
    // Step 0
    deBugFile << "entering connected8 method" << endl;

    // Step 1
    connect8Pass1(zeroFramedAry, newLabel, EQAry);
    deBugFile << "After connected8 pass1, newLabel =" << endl;
    printArray(newLabel, numRow, numCol, RFprettyPrintFile);
    imgReformat(zeroFramedAry, RFprettyPrintFile);
    printEQAry(EQAry, newLabel[newMax], RFprettyPrintFile);

    // Step 2
    connect8Pass2(zeroFramedAry, EQAry);
    deBugFile << "After connected8 pass2, newLabel =" << endl;
    printArray(newLabel, numRow, numCol, RFprettyPrintFile);
    imgReformat(zeroFramedAry, RFprettyPrintFile);
    printEQAry(EQAry, newLabel[newMax], RFprettyPrintFile);

    // Step 3
    int trueNumCC = manageEQAry(EQAry, newLabel);
    printEQAry(EQAry, newLabel[newMax], RFprettyPrintFile);
    int newMin = 0;
    int newMax = trueNumCC;
    CCproperty* CCprop = new CCproperty[trueNumCC+1];
    deBugFile << "In connected8, after manage EQAry, trueNumCC =" << trueNumCC << endl;

    // Step 4
    connectPass3(zeroFramedAry, EQAry, CCprop, trueNumCC, deBugFile);

    // Step 5
    imgReformat(zeroFramedAry, RFprettyPrintFile);

    // Step 6
    printEQAry(EQAry, newLabel[newMax], RFprettyPrintFile);

    // Step 7
    deBugFile << "Leaving connected8 method" << endl;
}

void connectPass3(int** zeroFramedAry, int* EQAry, ComponentProperty* CCproperty, int trueNumCC, ofstream& deBugFile) {
    deBugFile << "entering connectPass3 method" << endl;

    // initialize CCproperty for each connected component
    for (int i = 1; i <= trueNumCC; i++) {
        CCproperty[i].label = i;
        CCproperty[i].numPixels = 0;
        CCproperty[i].minR = numRow;
        CCproperty[i].maxR = 0;
        CCproperty[i].minC = numCol;
        CCproperty[i].maxC = 0;
    }

    // scan inside of the zeroFramedAry left-right & top-bottom
    for (int r = 1; r < numRow; r++) {
        for (int c = 1; c < numCol; c++) {
            int pixelValue = zeroFramedAry[r][c];

            if (pixelValue > 0) {
                // relabel the pixel
                zeroFramedAry[r][c] = EQAry[pixelValue];
                int label = zeroFramedAry[r][c];

                // update CCproperty for the connected component
                CCproperty[label].numPixels++;

                if (r < CCproperty[label].minR) {
                    CCproperty[label].minR = r;
                }
                if (r > CCproperty[label].maxR) {
                    CCproperty[label].maxR = r;
                }
                if (c < CCproperty[label].minC) {
                    CCproperty[label].minC = c;
                }
                if (c > CCproperty[label].maxC) {
                    CCproperty[label].maxC = c;
                }
            }
        }
    }

    deBugFile << "leaving connectPass3 method" << endl;
}
}
