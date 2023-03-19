#include <iostream>
#include <fstream>
#include <vector>
using namespace std;

// Function to check if the pixel at (x,y) is part of a component
bool is_component(vector<vector<int>>& image, int x, int y, int connectivity) {
    int rows = image.size();
    int cols = image[0].size();
    int color = image[x][y];

    if (color == 0) {
        return false; // Pixel is background
    }

    if (connectivity == 4) {
        if (x > 0 && image[x-1][y] == color) {
            return true; // Pixel has a 4-connected neighbor
        }
        if (y > 0 && image[x][y-1] == color) {
            return true; // Pixel has a 4-connected neighbor
        }
        if (x < rows-1 && image[x+1][y] == color) {
            return true; // Pixel has a 4-connected neighbor
        }
        if (y < cols-1 && image[x][y+1] == color) {
            return true; // Pixel has a 4-connected neighbor
        }
    }
    else if (connectivity == 8) {
        for (int i = -1; i <= 1; i++) {
            for (int j = -1; j <= 1; j++) {
                if (x+i >= 0 && x+i < rows && y+j >= 0 && y+j < cols && (i != 0 || j != 0)) {
                    if (image[x+i][y+j] == color) {
                        return true; // Pixel has an 8-connected neighbor
                    }
                }
            }
        }
    }

    return false; // Pixel has no connected neighbors
}

// Function to label the components in the image
vector<vector<int>> label_components(vector<vector<int>>& image, int connectivity) {
    int rows = image.size();
    int cols = image[0].size();
    int label = 1;
    vector<vector<int>> labeled_image(rows, vector<int>(cols, 0));

    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            if (is_component(image, i, j, connectivity) && labeled_image[i][j] == 0) {
                // Found a new component, label it and all its connected pixels
                labeled_image[i][j] = label;

                // Use a queue to perform a breadth-first search of the component
                vector<pair<int, int>> queue;
                queue.push_back(make_pair(i, j));

                while (!queue.empty()) {
                    int x = queue.front().first;
                    int y = queue.front().second;
                    queue.erase(queue.begin());

                    if (x > 0 && is_component(image, x-1, y, connectivity) && labeled_image[x-1][y] == 0) {
                        labeled_image[x-1][y] = label;
                        queue.push_back(make_pair(x-1, y));
                    }
                    if (y > 0 && is_component(image, x, y-1, connectivity) && labeled_image[x][y-1] == 0) {
                        labeled_image[x][y-1] = label;
                        queue.push_back(make_pair(x, y-1));
                    }
                    if (x < rows-1 && is_component(image, x-1, y, connectivity) && labeled_image[x+1][y] == 0) {
labeled_image[x+1][y] = label;
queue.push_back(make_pair(x+1, y));
}
if (y < cols-1 && is_component(image, x, y+1, connectivity) && labeled_image[x][y+1] == 0) {
labeled_image[x][y+1] = label;
queue.push_back(make_pair(x, y+1));
}
                if (connectivity == 8) {
                    if (x > 0 && y > 0 && is_component(image, x-1, y-1, connectivity) && labeled_image[x-1][y-1] == 0) {
                        labeled_image[x-1][y-1] = label;
                        queue.push_back(make_pair(x-1, y-1));
                    }
                    if (x > 0 && y < cols-1 && is_component(image, x-1, y+1, connectivity) && labeled_image[x-1][y+1] == 0) {
                        labeled_image[x-1][y+1] = label;
                        queue.push_back(make_pair(x-1, y+1));
                    }
                    if (x < rows-1 && y > 0 && is_component(image, x+1, y-1, connectivity) && labeled_image[x+1][y-1] == 0) {
                        labeled_image[x+1][y-1] = label;
                        queue.push_back(make_pair(x+1, y-1));
                    }
                    if (x < rows-1 && y < cols-1 && is_component(image, x+1, y+1, connectivity) && labeled_image[x+1][y+1] == 0) {
                        labeled_image[x+1][y+1] = label;
                        queue.push_back(make_pair(x+1, y+1));
                    }
                }
            }

            label++;
        }
    }
}

return labeled_image;
}

int main(int argc, char* argv[]) {
if (argc != 3) {
cout << "Usage: " << argv[0] << " <input_file> <connectivity>" << endl;
return 1;
}
// Read the image data from the input file
ifstream input_file(argv[1]);
if (!input_file.is_open()) {
    cout << "Error: could not open input file" << endl;
    return 1;
}

vector<vector<int>> image;
string line;
while (getline(input_file, line)) {
    vector<int> row;
    for (int i = 0; i < line.length(); i++) {
        row.push_back(line[i] - '0');
    }
    image.push_back(row);
}

input_file.close();

// Convert the image data if requested
char convert;
cout << "Do you want to convert the image data (Y/N)? ";
cin >> convert;

if (convert == 'Y' || convert == 'y') {
    for (int i = 0; i < image.size(); i++) {
        for (int j = 0; j < image[0].size(); j++) {
            image[i][j] = 1 - image[i][j]; // Invert the pixels
        }
    }
}

// Label the components in the image
int connectivity = atoi(argv[2]);
if (connectivity != 4 && connectivity != 8) {
    cout << "Error: invalid connectivity" << endl;
    return 1
}

vector<vector<int>> labeled_image = label_components(image, connectivity);

// Output the labeled image data
for (int i = 0; i < labeled_image.size(); i++) {
    for (int j = 0; j < labeled_image[0].size(); j++) {
        cout << labeled_image[i][j];
    }
    cout << endl;
}

return 0;
}

