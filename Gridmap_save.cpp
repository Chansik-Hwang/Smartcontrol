#include <iostream>
#include <vector>
#include <functional> //std::bind 포함
#include <string> 
#include <fstream>
#include <csignal> //ctrl+C 인식
#include <memory> // 스마트 포인터, std::make_shared 를 위함
#include "rclcpp/rclcpp.hpp"
#include "nav_msgs/msg/occupancy_grid.hpp"

volatile std::sig_atomic_t g_terminate = false; //ctrl+C를 통해 종료되었는지 flag변수 

std::vector<std::vector<int>> save;

class Gridconvert : public rclcpp::Node //생성자 작성, Node class 상속
{
    public:
        Gridconvert() : Node("Gridconvert")
        {   // rclcpp::QoS(rclcpp::SystemDefaultsQoS())
            auto qos_profile = rclcpp::QoS(rclcpp::KeepLast(10)); //QOS : quality of service, ROS1,2에서 통신이 잘 되게 해주는 것
            //위쪽에, 노드 간 통신 조정코드 
            subscriber_ = this->create_subscription<nav_msgs::msg::OccupancyGrid>(
                "/map",
                qos_profile,
                std::bind(&Gridconvert::mapConvert_Callback, this, std::placeholders::_1)); //Gridconvert의 mapconvert_callback함수를 객체로 생성
                //map이라는 토픽에서, nav_msgs::msg::OccupancyGrid type 데이터를 받아서 callback 함수에서 처리함 
        } 
    private:
        rclcpp::Subscription<nav_msgs::msg::OccupancyGrid>::SharedPtr subscriber_; //subscriber_를 가리키기 위한 스마트 포인터, 

        int ROW; //unsigned int 32bits
        int COL;
        int key_save;

        //메모리 관리, 수명 주기 관리를 위해 스마트 포인터로 선언. subscriber가 파괴되었을 때, subscribe 취소, 자원 해제

        void mapConvert_Callback(const nav_msgs::msg::OccupancyGrid::SharedPtr msg) //스마트 포인터 변수로 메세지 정보 받음
        {
            std::cout<<"msg.MetaData.width: "<< msg->info.width<<std::endl;
            std::cout<<"msg.MetaData.height: "<< msg->info.height<<std::endl;
            ROW = msg->info.height;
            COL = msg->info.width;
            std::cout << "ROW : " << ROW <<std::endl;

 
            std::cin >> key_save; 
            if(key_save == 1)
                SaveToFile(msg);
            else if(key_save == 0)
            {
                std::cout << "wait 1s" << std::endl;
                rclcpp::sleep_for(std::chrono::milliseconds(1000));
            }    
        }
        
        void SaveToFile(const nav_msgs::msg::OccupancyGrid::SharedPtr msg)
        {
            std::string file_path = "OccupancyMap.txt";

            std::ofstream writefile;
            writefile.open(file_path);
            if(writefile.is_open())
            {
                for(int i=0; i<ROW; i++)
                {
                    for(int j=0; j<COL; j++)
                    {
                        int index = i*COL + j;
                        writefile << (int)msg->data[index] << " ";
                    }
                    writefile << std::endl;
                }
                std::cout << "File Saved successfully at : " << file_path << std::endl;           
            }
            else
            {
                std::cerr << "Failed to open file!" << std::endl;
            }
            rclcpp::shutdown(); 
        }
};

//Susbscriber를 통해 생성된 그리드맵을 숫자로 변환하는 작업을 수행

int main(int argc, char** argv){

    rclcpp::init(argc, argv); //initiate the rclcpp client library
    auto node = std::make_shared<Gridconvert>(); // node 선언
    rclcpp::spin(node); //토픽이 들어올 때마다 큐에 요청된 콜백함수를 처리한다.
    return 0;
}