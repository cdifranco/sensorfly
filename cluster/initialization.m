close all
clear all

load ('./loading_files/distribution_table_0p5.mat')
direction_number = 4;%how many direction can each sensorfly take
trans_init_number = 1;
base_number_s = 5;
base_number_e = 5;
testing_round = 1000;
bel_threshold =0.0001;
port = 'COM5';

%measurements
time = [];
accuracy = [];
error_time = [];
