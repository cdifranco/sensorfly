close all;
load 'raw_data_3_8.mat';
reading_test_result = reading;
%% Find Valid Reading
for c = 1 : size(reading_test_result, 1)
    if isempty(find(reading_test_result(c,2:end) == inf,1)) 
        valid_reading(c, :) = reading_test_result(c, :);
    end
end
%% Node 21
node_21_reading = valid_reading(valid_reading(:,2)== 21, :);
node_21_timestamp = node_21_reading(:, 1);
node_21_reading_compass = node_21_reading(:, 3);
node_21_reading_sig_2 = node_21_reading(:,10);
node_21_reading_edist = zeros(1, size(node_21_reading, 1));
for c = 1 : size(node_21_reading, 1);
    node_21_reading_edist(c) = sqrt(sum(node_21_reading(c, 4:end).*node_21_reading(c, 4:end)));
end
plot(node_21_timestamp, node_21_reading_compass);
axis([min(node_21_timestamp), max(node_21_timestamp), 0, max(node_21_reading_compass)]);
datetick;
figure;
plot(node_21_timestamp, node_21_reading_sig_2);
axis([min(node_21_timestamp), max(node_21_timestamp), 0, max(node_21_reading_sig_2)]);
datetick;
figure;
plot(node_21_timestamp, node_21_reading_edist);
axis([min(node_21_timestamp), max(node_21_timestamp), 0, max(node_21_reading_edist)]);
datetick;
%% Node 22
node_22_reading = valid_reading(valid_reading(:,2)== 22, :);
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
%% Node 21
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
node_25_reading_sig_2 = node_25_reading(:,10);
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
