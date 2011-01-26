close all
clear all

load ('./loading_files/distribution_table_0p7.mat')
direction_number = 4;%how many direction can each sensorfly take
trans_init_number = 1;
base_number_s = 10;
base_number_e = 10;
testing_round = 50;
area_number = 27;
bel_threshold =0.0001;
port = 'COM11';
packet_id = floor(rand*254);
node_id = 2;
center_filter = 0.70;
%host = '192.168.1.112';
%measurements
time = [];
accuracy = [];
error_time = [];
