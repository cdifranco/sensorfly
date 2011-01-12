close all
clear all

load ('./loading_files/distribution_table_1p0.mat')
direction_number = 4;%how many direction can each sensorfly take
trans_init_number = 1;
base_number_s = 10;
base_number_e = 10;
testing_round = 1;
bel_threshold =0.0001;
port = 'COM5';
serial_port = serial(port,'BaudRate',38400,'DataBits',8,'Timeout', 2);
%measurements
time = [];
accuracy = [];
error_time = [];
