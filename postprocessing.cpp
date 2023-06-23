#include <iostream>
#include <fstream>
#include <vector>
#include <string>

 int ROW = 0;
 int COL = 0;

int const compressed_row = 60;
int const compressed_col = 80;
std::vector<std::vector <int>> Readtext(std::string filepath)
{
    std::ifstream fin;
    fin.open(filepath);

    if (fin.is_open())
    {
        fin >> ROW >> COL;
        std::vector<std::vector <int>> result(ROW, std::vector<int>(COL, 0));
        for (int i = 0; i < ROW; i++)
        {
            for (int j = 0; j < COL; j++)
            {
                fin >> result[i][j];
            }
        }
        fin.close();
        return result;
    }
    else
    {
        std::cout << "No file exist!" << std::endl;
        exit(0);
    }
}


int main()
{
    std::vector<std::vector <int>> Grid(ROW, std::vector<int>(COL, 0));
    std::vector<std::vector <int>> compressed_Grid(compressed_row, std::vector<int>(compressed_col, 0));

    std::string filepath = "C:\\C_C++_Projects\\map_postprocessing\\OccupancyMap.txt";
    Grid = Readtext(filepath);

    for(int i =0; i< ROW; i++) 
    {
        for(int j=0; j<COL; j++)
        {
            if(Grid[i][j] == -1)
            {
                Grid[i][j] == 100;
            }            
        }
    }

    for(int i = 0; i <compressed_row; i++ )
    {
        for(int j =0; j <compressed_col; j++ )
        {
            int startRow  = i*4;
            int endRow = (i+1)*4;
            int startCol = j*4;
            int endCol = (j+1)*4;

            if(startRow >= ROW) startRow = ROW-1;
            if(endRow >= ROW) endRow = ROW;
            if(startCol >= COL) startCol = COL - 1;
            if(endCol >= COL) endCol = COL;

            int sum =0, count = 0;

            for(int r = startRow; r < endRow; r++)
            {
                for(int c = startCol; c < endCol; c++)
                {

                   sum +=Grid[r][c];
                   count++;
                }               
            }

            int average = (count >0) ? sum /count : -7;
            if(average >= 10)
            {
               compressed_Grid[i][j] = 1;
            }
             else
            {
               compressed_Grid[i][j] = 0;
            }
        }
    }
    
    std::ofstream Writefile;
    Writefile.open("compressed_map.txt");
    if(Writefile.is_open())
    {
        for (int i=compressed_row-1; i>=0 ; i --)
        {
            for(int j=0; j<compressed_col ; j++)
            {
                Writefile <<compressed_Grid[i][j] << " ";
            }
            Writefile << std::endl;
        }
    }
}