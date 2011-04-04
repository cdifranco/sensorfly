close all;
load 'raw_data_3_14.mat';
reading_test_result = reading;
%% Find Valid Reading
for c = 1 : size(reading_test_result, 1)
    if isempty(find(reading_test_result(c,2:end) == inf,1)) 
        valid_reading(c, :) = reading_test_result(c, :);
    end
end

%% Node 21
% signatures and readings
node_21_reading = valid_reading(valid_reading(:,2)== 21, :);% change here 21 --> 2? to see other fig
node_21_timestamp = node_21_reading(:, 1);
node_21_reading_compass = node_21_reading(:, 3);
node_21_reading_sig_1 = node_21_reading(:,4);
node_21_reading_sig_2 = node_21_reading(:,5);
node_21_reading_sig_3 = node_21_reading(:,6);
node_21_reading_sig_4 = node_21_reading(:,7);
node_21_reading_sig_5 = node_21_reading(:,8);
node_21_reading_sig_6 = node_21_reading(:,9);
node_21_reading_sig_7 = node_21_reading(:,10);
node_21_reading_sig_8 = node_21_reading(:,11);
node_21_reading_sig_9 = node_21_reading(:,12);
node_21_reading_sig_10 = node_21_reading(:,13);
node_21_reading_edist = zeros(1, size(node_21_reading, 1));
for c = 1 : size(node_21_reading, 1);
    node_21_reading_edist(c) = sqrt(sum(node_21_reading(c, 4:end).*node_21_reading(c, 4:end)));
end
% standard deviation
std_sig_1 = std(node_21_reading_sig_1);
mean_sig_1 = mean(node_21_reading_sig_1);
std_sig_2 = std(node_21_reading_sig_2);
mean_sig_2 = mean(node_21_reading_sig_2);
std_sig_3 = std(node_21_reading_sig_3);
mean_sig_3 = mean(node_21_reading_sig_3);
std_sig_4 = std(node_21_reading_sig_4);
mean_sig_4 = mean(node_21_reading_sig_4);
std_sig_5 = std(node_21_reading_sig_5);
mean_sig_5 = mean(node_21_reading_sig_5);
std_sig_6 = std(node_21_reading_sig_6);
mean_sig_6 = mean(node_21_reading_sig_6);
std_sig_7 = std(node_21_reading_sig_7);
mean_sig_7 = mean(node_21_reading_sig_7);
std_sig_8 = std(node_21_reading_sig_8);
mean_sig_8 = mean(node_21_reading_sig_8);
std_sig_9 = std(node_21_reading_sig_9);
mean_sig_9 = mean(node_21_reading_sig_9);
std_sig_10 = std(node_21_reading_sig_10);
mean_sig_10 = mean(node_21_reading_sig_10);
std_edist = std(node_21_reading_edist);
mean_edist = mean(node_21_reading_edist);
percentage = [std_sig_1/mean_sig_1, std_sig_2/mean_sig_2, std_sig_3/mean_sig_3, std_sig_4/mean_sig_4, std_sig_5/mean_sig_5, std_sig_6/mean_sig_6, std_sig_7/mean_sig_7,...
    std_sig_8/mean_sig_8, std_sig_9/mean_sig_9, std_sig_10/mean_sig_10]


bar(percentage);
hold on;
plot([0, size(percentage, 2)+1],[std_edist/mean_edist, std_edist/mean_edist], 'r');
axis([0, size(percentage, 2)+1, 0, max(percentage)]);
hold off;
figure;
% plot everything
plot(node_21_timestamp, node_21_reading_compass);
axis([min(node_21_timestamp), max(node_21_timestamp), 0, max(node_21_reading_compass)]);
datetick;
figure;
%%
subplot(3,1,1);
plot(node_21_timestamp, node_21_reading_edist/max(node_21_reading_edist));
axis([min(node_21_timestamp), max(node_21_timestamp), 0, 1]);% max(node_21_reading_edist)]);
datetick;
subplot(3,1,2);
plot(node_21_timestamp, node_21_reading_sig_1/max(node_21_reading_sig_1));
axis([min(node_21_timestamp), max(node_21_timestamp), 0, 1]);% max(node_21_reading_sig_1)]);
datetick;
subplot(3,1,3);
plot(node_21_timestamp, node_21_reading_sig_9/max(node_21_reading_sig_9));
axis([min(node_21_timestamp), max(node_21_timestamp), 0, 1]);% max(node_21_reading_sig_9)]);
datetick;
figure;
subplot(3,1,1);
plot(node_21_timestamp, node_21_reading_edist);
axis([min(node_21_timestamp), max(node_21_timestamp), 0, max(node_21_reading_edist)]);
datetick;
subplot(3,1,2);
plot(node_21_timestamp, node_21_reading_sig_1);
axis([min(node_21_timestamp), max(node_21_timestamp), 0, max(node_21_reading_sig_1)]);
datetick;
subplot(3,1,3);
plot(node_21_timestamp, node_21_reading_sig_9);
axis([min(node_21_timestamp), max(node_21_timestamp), 0, max(node_21_reading_sig_9)]);
datetick;

%{
subplot(5,7,[1:5 8:12 15:19 22:26 29:33]);
plot(node_21_timestamp, node_21_reading_edist);
axis([min(node_21_timestamp), max(node_21_timestamp), 0, max(node_21_reading_edist)]);
datetick;
subplot(5,7,6);
plot(node_21_timestamp, node_21_reading_sig_1);
axis([min(node_21_timestamp), max(node_21_timestamp), 0, max(node_21_reading_sig_1)]);
datetick('keeplimits','keepticks');
subplot(5,7,7);
plot(node_21_timestamp, node_21_reading_sig_2);
axis([min(node_21_timestamp), max(node_21_timestamp), 0, max(node_21_reading_sig_2)]);
datetick('keeplimits','keepticks');
subplot(5,7,13);
plot(node_21_timestamp, node_21_reading_sig_3);
axis([min(node_21_timestamp), max(node_21_timestamp), 0, max(node_21_reading_sig_3)]);
datetick('keeplimits','keepticks');
subplot(5,7,14);
plot(node_21_timestamp, node_21_reading_sig_4);
axis([min(node_21_timestamp), max(node_21_timestamp), 0, max(node_21_reading_sig_4)]);
datetick('keeplimits','keepticks');
subplot(5,7,20);
plot(node_21_timestamp, node_21_reading_sig_5);
axis([min(node_21_timestamp), max(node_21_timestamp), 0, max(node_21_reading_sig_5)]);
datetick('keeplimits','keepticks');
subplot(5,7,21);
plot(node_21_timestamp, node_21_reading_sig_6);
axis([min(node_21_timestamp), max(node_21_timestamp), 0, max(node_21_reading_sig_6)]);
datetick('keeplimits','keepticks');
subplot(5,7,27);
plot(node_21_timestamp, node_21_reading_sig_7);
axis([min(node_21_timestamp), max(node_21_timestamp), 0, max(node_21_reading_sig_7)]);
datetick('keeplimits','keepticks');
subplot(5,7,28);
plot(node_21_timestamp, node_21_reading_sig_8);
axis([min(node_21_timestamp), max(node_21_timestamp), 0, max(node_21_reading_sig_8)]);
datetick('keeplimits','keepticks');
subplot(5,7,34);
plot(node_21_timestamp, node_21_reading_sig_9);
axis([min(node_21_timestamp), max(node_21_timestamp), 0, max(node_21_reading_sig_9)]);
datetick('keeplimits','keepticks');
subplot(5,7,35);
plot(node_21_timestamp, node_21_reading_sig_10);
axis([min(node_21_timestamp), max(node_21_timestamp), 0, max(node_21_reading_sig_10)]);
datetick('keeplimits','keepticks');  
%}

%{
%% Node 22
node_22_reading = valid_reading(valid_reading(:,2)== 21, :);
node_22_timestamp = node_22_reading(:, 1);
node_22_reading_compass = node_22_reading(:, 3);
node_22_reading_sig_2 = node_22_reading(:,10);
node_22_reading_edist = zeros(1, size(node_22_reading, 1));
for c = 1 : size(node_22_reading, 1);
    node_22_reading_edist(c) = sqrt(sum(node_22_reading(c, 4:end).*node_22_reading(c, 4:end)));
end
figure;
plot(node_22_timestamp, node_22_reading_compass);
axis([min(node_22_timestamp), max(node_22_timestamp), 0, max(node_22_reading_compass)]);
datetick;
figure;
plot(node_22_timestamp, node_22_reading_sig_2);
axis([min(node_22_timestamp), max(node_22_timestamp), 0, max(node_22_reading_sig_2)]);
datetick;
figure;
plot(node_22_timestamp, node_22_reading_edist);
axis([min(node_22_timestamp), max(node_22_timestamp), 0, max(node_22_reading_edist)]);
datetick;


%% Node 23
node_23_reading = valid_reading(valid_reading(:,2)== 23, :);
node_23_timestamp = node_23_reading(:, 1);
node_23_reading_compass = node_23_reading(:, 3);
node_23_reading_sig_2 = node_23_reading(:,10);
node_23_reading_edist = zeros(1, size(node_23_reading, 1));
for c = 1 : size(node_23_reading, 1);
    node_23_reading_edist(c) = sqrt(sum(node_23_reading(c, 4:end).*node_23_reading(c, 4:end)));
end
figure;
plot(node_23_timestamp, node_23_reading_compass);
axis([min(node_23_timestamp), max(node_23_timestamp), 0, max(node_23_reading_compass)]);
datetick;
figure;
plot(node_23_timestamp, node_23_reading_sig_2);
axis([min(node_23_timestamp), max(node_23_timestamp), 0, max(node_23_reading_sig_2)]);
datetick;
figure;
plot(node_23_timestamp, node_23_reading_edist);
axis([min(node_23_timestamp), max(node_23_timestamp), 0, max(node_23_reading_edist)]);
datetick;

%% Node 24
node_24_reading = valid_reading(valid_reading(:,2)== 24, :);
node_24_timestamp = node_24_reading(:, 1);
node_24_reading_compass = node_24_reading(:, 3);
node_24_reading_sig_2 = node_24_reading(:,10);
node_24_reading_edist = zeros(1, size(node_24_reading, 1));
for c = 1 : size(node_24_reading, 1);
    node_24_reading_edist(c) = sqrt(sum(node_24_reading(c, 4:end).*node_24_reading(c, 4:end)));
end
figure;
plot(node_24_timestamp, node_24_reading_compass);
axis([min(node_24_timestamp), max(node_24_timestamp), 0, max(node_24_reading_compass)]);
datetick;
figure;
plot(node_24_timestamp, node_24_reading_sig_2);
axis([min(node_24_timestamp), max(node_24_timestamp), 0, max(node_24_reading_sig_2)]);
datetick;
figure;
plot(node_24_timestamp, node_24_reading_edist);
axis([min(node_24_timestamp), max(node_24_timestamp), 0, max(node_24_reading_edist)]);
datetick;

%% Node 25
node_25_reading = valid_reading(valid_reading(:,2)== 25, :);
node_25_timestamp = node_25_reading(:, 1);
node_25_reading_compass = node_25_reading(:, 3);
node_25_reading_sig_2 = node_25_reading(:,13);
node_25_reading_edist = zeros(1, size(node_25_reading, 1));
for c = 1 : size(node_25_reading, 1);
    node_25_reading_edist(c) = sqrt(sum(node_25_reading(c, 4:end).*node_25_reading(c, 4:end)));
end
figure;
plot(node_25_timestamp, node_25_reading_compass);
axis([min(node_25_timestamp), max(node_25_timestamp), 0, max(node_25_reading_compass)]);
datetick;
figure;
plot(node_25_timestamp, node_25_reading_sig_2);
axis([min(node_25_timestamp), max(node_25_timestamp), 0, max(node_25_reading_sig_2)]);
datetick;
figure;
plot(node_25_timestamp, node_25_reading_edist);
axis([min(node_25_timestamp), max(node_25_timestamp), 0, max(node_25_reading_edist)]);
datetick;
%}