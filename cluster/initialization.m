close all
clear all
load ('./loading_files/distribution_table_1p5')
load ('./loading_files/get_reading_data');
load ('./loading_files/room');

%% 
main_loop_count = 10;%how many sensorflies going in
path_reading_count = 200;% how many readings each sensorfly will take
step_len = 1.5;%how long will each step take
direction_number = 4;%how many direction can each sensorfly take
trans_init_number = 1;
bel_threshold = 0.025;

%measurements
time = [];
accuracy = [];
%center_count = [];
%error_time = [];