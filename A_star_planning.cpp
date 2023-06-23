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


//직선             아래 위 오 왼
const int fw_dx[4] = {1,-1,0,0};
const int fw_dy[4] = {0,0,1,-1}; 
//대각선            동남, 서남, 북동, 북서
const int dia_dx[4] = {1,1,-1,-1};
const int dia_dy[4] = {1,-1,1,-1};


using Pair = std::pair<int, int>;
using Info = std::pair<double, Pair>;

char FinalGrid[MAX][MAX]; // 2차원 배열이, Astar를 실행하면 넘겨줄 수 있게끔. 


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

    while( !((cells[x][y].CurrentnodeInfo.second.first == 0) && (cells[x][y].CurrentnodeInfo.second.second == 0)) ) //src가 아닐동안
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
    std::vector<std::vector <bool>> closedlist(ROW, std::vector <bool>(COL,0)); //closedlist에 각 cell이 들어가 있는가 판단
    find_src_dst(Grid, dst, src); //src, dst 설정
    Pair current = src;    

    
    for (int i = 0; i < ROW; i++)
    {
        for (int j = 0; j < COL; j++)
        {
            cells[i][j].f_cost = cells[i][j].g_cost = cells[i][j].h_cost = 0.0;
        }
    }

    cells[src.first][src.second].CurrentnodeInfo.second.first = 0;
    cells[src.first][src.second].CurrentnodeInfo.second.second = 0; //src노드의 parent node를 자기자신으로!


    openlist.insert({ cells[src.first][src.second].f_cost, src}); // openlist 초기화

    closedlist[src.first][src.second] = true; // src노드 closedlist추가

    
    while (!openlist.empty())
    {
        
        std::set<Info>::iterator iter= openlist.begin(); //openlist 첫 요소

        closedlist[(*iter).second.first][(*iter).second.second] = true; //closed list에 저장      

        current.first = (*iter).second.first;
        current.second = (*iter).second.second;

    
        openlist.erase(iter); //openlist에서 제거  : 코드 안되면 제거하고 반복자 위치 고려하기

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

                if(is_blocked(Grid[next_x][next_y])) //, blank 함수도 있어야함
                {   
                    continue;
                }       
                else if (closedlist[next_x][next_y]) // closedlist안에 이미 있을 경우
                {
                    continue;
                }
                else if(!closedlist[next_x][next_y]) // closedlist안에 없는 경우
                {

                    cells[next_x][next_y].g_cost = cells[current.first][current.second].g_cost+ 1;
                    Heuristic_Cal(next_x, next_y,dst);
                    cells[next_x][next_y].f_cost = cells[next_x][next_y].g_cost + cells[next_x][next_y].h_cost;


                    for(iter = openlist.begin(); iter != openlist.end(); iter++) //openlist에 있는지 조사
                    {
                        if( (*iter).second.first == next_x && (*iter).second.second == next_y  ) // openlist에 있다면
                        {
                            In_openlist = true;
                            if((*iter).first > cells[next_x][next_y].f_cost) // fcost가 작을 경우
                            {
                                cells[next_x][next_y].CurrentnodeInfo.second.first = current.first; //parent node setting
                                cells[next_x][next_y].CurrentnodeInfo.second.second = current.second;//parent node setting
                                openlist.erase(iter);
                                openlist.insert({cells[next_x][next_y].f_cost, {next_x,next_y}} );
                            }    
                            break; // 추후확인
                        }
                    }
                    if(In_openlist == true) 
                        continue;
                    else //not in openlist
                    {
                        cells[next_x][next_y].CurrentnodeInfo.second.first = current.first; //parent node setting
                        cells[next_x][next_y].CurrentnodeInfo.second.second = current.second;//parent node setting
                        openlist.insert({cells[next_x][next_y].f_cost, {next_x,next_y}} ); //openlist에 없는 노드 추가(parent node가 포함)
                    }
                }
            }

            for(int i=0; i<4; i++)//diagonal direction
            {
                int next_x = current.first+dia_dx[i];
                int next_y = current.second+dia_dy[i];
                bool In_openlist = false;

                if(is_blocked(Grid[next_x][next_y])) //, blank 함수도 있어야함
                {   
                    continue;
                }       
                else if (closedlist[next_x][next_y]) // closedlist 의 변수들 좌표 == current node로부터 움직인 좌표인가, 반복문 안쓰고 어떻게? 
                {
                    continue;
                }
                else if(!closedlist[next_x][next_y]) // closedlist에 있는지 판단
                {

                    cells[next_x][next_y].g_cost = cells[current.first][current.second].g_cost + sqrt(2);
                    Heuristic_Cal(next_x, next_y,dst);
                    cells[next_x][next_y].f_cost = cells[next_x][next_y].g_cost + cells[next_x][next_y].h_cost;

                    for(iter = openlist.begin(); iter != openlist.end(); iter++) //openlist에 있는지 조사
                    {
                        if( (*iter).second.first == next_x && (*iter).second.second == next_y ) // openlist에 있다면
                        {    
                            In_openlist = true;                        
                            if((*iter).first > cells[next_x][next_y].f_cost) // fcost가 작을 경우
                            {
                                cells[next_x][next_y].CurrentnodeInfo.second.first = current.first; //parent node설정
                                cells[next_x][next_y].CurrentnodeInfo.second.second = current.second;//parent node 설정
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
                        cells[next_x][next_y].CurrentnodeInfo.second.first = current.first; //parent node설정
                        cells[next_x][next_y].CurrentnodeInfo.second.second = current.second;//parent node 설정                        
                        openlist.insert({cells[next_x][next_y].f_cost, {next_x,next_y}} ); //openlist에 없는 노드 추가(parent node가 포함)                
                    }    

                }

            }

        }

    }
    std::cout << "코드 오류" << std::endl;
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