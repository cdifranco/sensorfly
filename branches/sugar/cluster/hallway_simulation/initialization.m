close all
clear all
load ('./loading_files/distribution_table_1p0.mat')
load ('./loading_files/get_reading_data.mat');
load ('./loading_files/room.mat');
load ('./loading_files/base.mat');
%load ('./loading_files/path_10000.mat');
%% 
main_loop_count = 10;%how many sensorflies going in
path_reading_count = 200;% how many readings each sensorfly will take
step_len = 1.5;%how long will each step take
direction_number = 4;%how many direction can each sensorfly take
trans_init_number = 1;
base_number_s = 12;
base_number_e = 12;
testing_round = 10000;
bel_threshold =0.0001;

%measurements
time = [];
accuracy = [];
error_time = [];