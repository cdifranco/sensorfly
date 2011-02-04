%{
%%
load 'raw_data.mat';
std_dev = [];
for rc = 1 : size(reading,1)/20
   sds = [];
   for i = 5 : 14
      sd = std(reading((rc-1)*20+1:rc*20,i));
      sds = [sds sd];
   end
   std_dev = [std_dev; sds];
end
save 'sig_analysis.mat';
%%
clear all;
load 'processed_data.mat';
load 'sig_analysis.mat';
edist = 0;
sig_euclid = [];
for sc = 1 : size(signatures,1)
    edist = sqrt(sum(signatures(sc,5:end).*signatures(sc,5:end)));
    sig_euclid = [sig_euclid; signatures(sc,1) signatures(sc,2) signatures(sc,5:end) edist];
end
scatter(sig_euclid(:,1), sig_euclid(:,2),75,sig_euclid(:,end), 'Filled');
save 'sig_analysis.mat'
%}
%%
load 'reading_test.mat';
reading_test_result = reading_test_result(1: mainloop,:);
%% Node 21

for c = 1 : size(reading_test_result, 1)
    if isempty(find(reading_test_result(c,2:end) == inf,1)) 
        valid_reading(c, :) = reading_test_result(c, :);
    end
end
node_21_reading = valid_reading(valid_reading(:,2)== 25, :);
node_21_timestamp = node_21_reading(:, 1);
node_21_reading_compass = node_21_reading(:, 3);
node_21_reading_sig_2 = node_21_reading(:, 5);
node_21_reading_edist = zeros(1, size(node_21_reading, 1));
for c = 1 : size(node_21_reading, 1);
    node_21_reading_edist(c) = sqrt(sum(node_21_reading(c, 4:end).*node_21_reading(c, 4:end)));
end
plot(node_21_timestamp, node_21_reading_compass);
datetick;
figure;
plot(node_21_timestamp, node_21_reading_sig_2);
datetick;
figure;
plot(node_21_timestamp, node_21_reading_edist);
datetick;
save 'reading_test_analysis.mat';