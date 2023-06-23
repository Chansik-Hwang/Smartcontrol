#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <stack>
#include <utility>
#include <cmath>
#include <set>

#define MAX 100

int ROW = 0;
int COL = 0;


//����             �Ʒ� �� �� ��
const int fw_dx[4] = {1,-1,0,0};
const int fw_dy[4] = {0,0,1,-1}; 
//�밢��            ����, ����, �ϵ�, �ϼ�
const int dia_dx[4] = {1,1,-1,-1};
const int dia_dy[4] = {1,-1,1,-1};


using Pair = std::pair<int, int>;
using Info = std::pair<double, Pair>;

char FinalGrid[MAX][MAX]; // 2���� �迭��, Astar�� �����ϸ� �Ѱ��� �� �ְԲ�. 


struct Nodeinfo {
    double f_cost,g_cost,h_cost;
    Info CurrentnodeInfo;
};

Nodeinfo cells[MAX][MAX];


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
        return result;
    }
    else
    {
        std::cout << "No file exist!" << std::endl;
        exit(0);
    }
}


void find_src_dst(std::vector<std::vector <int>>& Grid, Pair& dst, Pair& src)
{
    for (int i = 0; i < ROW; i++)
    {
        for (int j = 0; j < COL; j++)
        {
            if (Grid[i][j] == 2)
            {
                src.first = i;
                src.second = j;
            }
            else if (Grid[i][j] == 3)
            {
                dst.first = i;
                dst.second = j;
            }
        }
    }
}

void Heuristic_Cal(int &nx, int &ny, Pair &dst)
{
    cells[nx][ny].h_cost = std::sqrt(std::pow(dst.first - nx, 2) + std::pow(dst.second - ny, 2));
}

bool is_destination(Pair current, Pair dst)
{
    if (current.first == dst.first && current.second == dst.second)
        return true;
    else
        return false;
}

bool is_blocked(int cellvalue)
{
    if (cellvalue == -1 || cellvalue == 1) 
        return 1;
    else
        return 0;
}

void Tracepath(Pair dst, Pair src)
{
    std::stack <Pair> Resultpath;
    int x = dst.first;
    int y = dst.second; 

    while( !((cells[x][y].CurrentnodeInfo.second.first == 0) && (cells[x][y].CurrentnodeInfo.second.second == 0)) ) //src�� �ƴҵ���
    {
        Resultpath.push({x,y});        
        x = cells[Resultpath.top().first][Resultpath.top().second].CurrentnodeInfo.second.first;
        y = cells[Resultpath.top().first][Resultpath.top().second].CurrentnodeInfo.second.second;
    }
    std::cout << std::endl;
    std::cout << "=====The result A* path=====" << std::endl;

    while(!Resultpath.empty())
    {  
        x = Resultpath.top().first;
        y = Resultpath.top().second;

        if(!(x == dst.first && y == dst.second))
        {
            FinalGrid[x][y] = '*';
            std::cout << "{" << x << "," << y << "}" << " <- ";  // stackcell 
            Resultpath.pop();
        }
        else
        {
            std::cout << "{" << x << "," << y << "}";  // stackcell 
            Resultpath.pop();
        }
    }
    std::cout << std::endl;
}


bool Astar_Algorithm(std::vector<std::vector <int>> &Grid)
{
    Pair dst;
    Pair src;
    std::set <Info> openlist;
    std::vector<std::vector <bool>> closedlist(ROW, std::vector <bool>(COL,0)); //closedlist�� �� cell�� �� �ִ°� �Ǵ�
    find_src_dst(Grid, dst, src); //src, dst ����
    Pair current = src;    

    
    for (int i = 0; i < ROW; i++)
    {
        for (int j = 0; j < COL; j++)
        {
            cells[i][j].f_cost = cells[i][j].g_cost = cells[i][j].h_cost = 0.0;
        }
    }

    cells[src.first][src.second].CurrentnodeInfo.second.first = 0;
    cells[src.first][src.second].CurrentnodeInfo.second.second = 0; //src����� parent node�� �ڱ��ڽ�����!


    openlist.insert({ cells[src.first][src.second].f_cost, src}); // openlist �ʱ�ȭ

    closedlist[src.first][src.second] = true; // src��� closedlist�߰�

    
    while (!openlist.empty())
    {
        
        std::set<Info>::iterator iter= openlist.begin(); //openlist ù ���

        closedlist[(*iter).second.first][(*iter).second.second] = true; //closed list�� ����      

        current.first = (*iter).second.first;
        current.second = (*iter).second.second;

    
        openlist.erase(iter); //openlist���� ����  : �ڵ� �ȵǸ� �����ϰ� �ݺ��� ��ġ ����ϱ�

        if (is_destination(current, dst)) // 
        {
            Tracepath(dst,src);
            return true;
        }
        else
        {   
            for(int i=0; i<4; i++)
            {
                int next_x = current.first+fw_dx[i];
                int next_y = current.second+fw_dy[i];
                bool In_openlist = false;

                if(is_blocked(Grid[next_x][next_y])) //, blank �Լ��� �־����
                {   
                    continue;
                }       
                else if (closedlist[next_x][next_y]) // closedlist�ȿ� �̹� ���� ���
                {
                    continue;
                }
                else if(!closedlist[next_x][next_y]) // closedlist�ȿ� ���� ���
                {

                    cells[next_x][next_y].g_cost = cells[current.first][current.second].g_cost+ 1;
                    Heuristic_Cal(next_x, next_y,dst);
                    cells[next_x][next_y].f_cost = cells[next_x][next_y].g_cost + cells[next_x][next_y].h_cost;


                    for(iter = openlist.begin(); iter != openlist.end(); iter++) //openlist�� �ִ��� ����
                    {
                        if( (*iter).second.first == next_x && (*iter).second.second == next_y  ) // openlist�� �ִٸ�
                        {
                            In_openlist = true;
                            if((*iter).first > cells[next_x][next_y].f_cost) // fcost�� ���� ���
                            {
                                cells[next_x][next_y].CurrentnodeInfo.second.first = current.first; //parent node setting
                                cells[next_x][next_y].CurrentnodeInfo.second.second = current.second;//parent node setting
                                openlist.erase(iter);
                                openlist.insert({cells[next_x][next_y].f_cost, {next_x,next_y}} );
                            }    
                            break; // ����Ȯ��
                        }
                    }
                    if(In_openlist == true) 
                        continue;
                    else //not in openlist
                    {
                        cells[next_x][next_y].CurrentnodeInfo.second.first = current.first; //parent node setting
                        cells[next_x][next_y].CurrentnodeInfo.second.second = current.second;//parent node setting
                        openlist.insert({cells[next_x][next_y].f_cost, {next_x,next_y}} ); //openlist�� ���� ��� �߰�(parent node�� ����)
                    }
                }
            }

            for(int i=0; i<4; i++)//diagonal direction
            {
                int next_x = current.first+dia_dx[i];
                int next_y = current.second+dia_dy[i];
                bool In_openlist = false;

                if(is_blocked(Grid[next_x][next_y])) //, blank �Լ��� �־����
                {   
                    continue;
                }       
                else if (closedlist[next_x][next_y]) // closedlist �� ������ ��ǥ == current node�κ��� ������ ��ǥ�ΰ�, �ݺ��� �Ⱦ��� ���? 
                {
                    continue;
                }
                else if(!closedlist[next_x][next_y]) // closedlist�� �ִ��� �Ǵ�
                {

                    cells[next_x][next_y].g_cost = cells[current.first][current.second].g_cost + sqrt(2);
                    Heuristic_Cal(next_x, next_y,dst);
                    cells[next_x][next_y].f_cost = cells[next_x][next_y].g_cost + cells[next_x][next_y].h_cost;

                    for(iter = openlist.begin(); iter != openlist.end(); iter++) //openlist�� �ִ��� ����
                    {
                        if( (*iter).second.first == next_x && (*iter).second.second == next_y ) // openlist�� �ִٸ�
                        {    
                            In_openlist = true;                        
                            if((*iter).first > cells[next_x][next_y].f_cost) // fcost�� ���� ���
                            {
                                cells[next_x][next_y].CurrentnodeInfo.second.first = current.first; //parent node����
                                cells[next_x][next_y].CurrentnodeInfo.second.second = current.second;//parent node ����
                                openlist.erase(iter);
                                openlist.insert({cells[next_x][next_y].f_cost, {next_x,next_y}} );
                            }    
                            break; 
                        }
                    }
                    if(In_openlist == true) 
                        continue;
                    else
                    {
                        cells[next_x][next_y].CurrentnodeInfo.second.first = current.first; //parent node����
                        cells[next_x][next_y].CurrentnodeInfo.second.second = current.second;//parent node ����                        
                        openlist.insert({cells[next_x][next_y].f_cost, {next_x,next_y}} ); //openlist�� ���� ��� �߰�(parent node�� ����)                
                    }    

                }

            }

        }

    }
    std::cout << "�ڵ� ����" << std::endl;
    return false;
}



void resultprint()
{
    for (int i = 0; i < ROW; i++)
    {
        for (int j = 0; j < COL; j++)
        {
            std::cout << FinalGrid[i][j] << " ";
        }
        std::cout << std::endl;
    }
}


int main()
{
    std::vector<std::vector <int>> Grid(ROW, std::vector<int>(COL, 0));

    std::string filepath = "C:\\C_C++_Projects\\Grid_Astar\\postprocessed_map.txt";
    Grid = Readtext(filepath);

    for (int i = 0; i < ROW; i++)
    {
        for (int j = 0; j < COL; j++)
        {
            FinalGrid[i][j] = Grid[i][j] + '0';
        }
    }

    for(int i=0; i<ROW; i++)
    {
        for(int j=0; j<COL; j++)
        {
            std::cout << Grid[i][j] << " ";        
        }
        std::cout<<std::endl;
    }
    
    if (Astar_Algorithm(Grid) == 0)
    {
        std::cout << "Astar failed : No optimal path" << std::endl;
        return -1;
    }
    else
    {
        resultprint();
    }

}