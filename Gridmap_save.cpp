#include <iostream>
#include <vector>
#include <functional> //std::bind ����
#include <string> 
#include <fstream>
#include <csignal> //ctrl+C �ν�
#include <memory> // ����Ʈ ������, std::make_shared �� ����
#include "rclcpp/rclcpp.hpp"
#include "nav_msgs/msg/occupancy_grid.hpp"

volatile std::sig_atomic_t g_terminate = false; //ctrl+C�� ���� ����Ǿ����� flag���� 

std::vector<std::vector<int>> save;

class Gridconvert : public rclcpp::Node //������ �ۼ�, Node class ���
{
    public:
        Gridconvert() : Node("Gridconvert")
        {   // rclcpp::QoS(rclcpp::SystemDefaultsQoS())
            auto qos_profile = rclcpp::QoS(rclcpp::KeepLast(10)); //QOS : quality of service, ROS1,2���� ����� �� �ǰ� ���ִ� ��
            //���ʿ�, ��� �� ��� �����ڵ� 
            subscriber_ = this->create_subscription<nav_msgs::msg::OccupancyGrid>(
                "/map",
                qos_profile,
                std::bind(&Gridconvert::mapConvert_Callback, this, std::placeholders::_1)); //Gridconvert�� mapconvert_callback�Լ��� ��ü�� ����
                //map�̶�� ���ȿ���, nav_msgs::msg::OccupancyGrid type �����͸� �޾Ƽ� callback �Լ����� ó���� 
        } 
    private:
        rclcpp::Subscription<nav_msgs::msg::OccupancyGrid>::SharedPtr subscriber_; //subscriber_�� ����Ű�� ���� ����Ʈ ������, 

        int ROW; //unsigned int 32bits
        int COL;
        int key_save;

        //�޸� ����, ���� �ֱ� ������ ���� ����Ʈ �����ͷ� ����. subscriber�� �ı��Ǿ��� ��, subscribe ���, �ڿ� ����

        void mapConvert_Callback(const nav_msgs::msg::OccupancyGrid::SharedPtr msg) //����Ʈ ������ ������ �޼��� ���� ����
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

//Susbscriber�� ���� ������ �׸������ ���ڷ� ��ȯ�ϴ� �۾��� ����

int main(int argc, char** argv){

    rclcpp::init(argc, argv); //initiate the rclcpp client library
    auto node = std::make_shared<Gridconvert>(); // node ����
    rclcpp::spin(node); //������ ���� ������ ť�� ��û�� �ݹ��Լ��� ó���Ѵ�.
    return 0;
}