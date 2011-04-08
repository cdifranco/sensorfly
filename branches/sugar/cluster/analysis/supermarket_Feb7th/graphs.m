close all;
clear all;

load 'ranging_test_2_7_15_40_cross';
bar(success_rate(1:5, 4),.5);

clear all;

load 'ranging_test_2_7_15_30_2_rack.mat';
figure;
bar(success_rate(:, 2), success_rate(:, 4),'y');

clear all;

load '1st_rack.mat';
figure;
bar(s_rate_filtered(:, 2), s_rate_filtered(:, 4),'y');





