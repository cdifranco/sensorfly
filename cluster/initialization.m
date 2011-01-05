close all
clear all
%% 
direction_number = 4;%how many direction can each sensorfly take
trans_init_number = 1;
base_number_s = 1;
base_number_e = 1;
testing_round = 1000;
bel_threshold =0.0001;
port = 'COM5';

%measurements
time = [];
accuracy = [];
error_time = [];
