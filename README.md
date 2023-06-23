# Smartcontrol
Code for applying A-star based on ROS2 Turtlebot3 SLAM

1. Gridmap_save : Code for making ROS2 node
- Get Gridmap information from map topic during SLAM 
- output is text file : OccupancyMap.txt

2. Gridmap_plot.ipynb : Plotting 2D gridmap textfile in Google Colab

3. postprocessing.cpp : postprocess the raw 2D gridmap
- OccupancyMap.txt -> postprocessed_map.txt

4. A_star_planning : do path planning  with postprocessed_map.txt

