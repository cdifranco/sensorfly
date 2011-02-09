% Select 3000 Readings for Clustering
%% Initialization
clear all;
close all;
load ('processed_data_supermarket.mat');
load ('distribution_table.mat'); % contains 7 distribution tables
distr_init = 5;
distribution_table_id = distr_init; % start with 0p9
size_remain = 1; % to record the continuous staying step in one cluster
size_remain_threshold = 5;
size_change = 0;  % to record the continuous changing step in multiple clusters
size_change_threshold = 5;
last_cluster = 0; % record last_cluster for dynamic changing of threshold
direction_number = 4; % how many direction can each sensorfly take
trans_init_number = 1; % initiate number of transition histroy
center = []; % ctr: cluster_id, contain_reading_number, real_x, real_y, sig1, sig2, sig3 ... sigN
trans_history = []; % transition history     
bel = []; % belief
bel_threshold = 0.0001; % threshold of belief
std_threshold = 2; % threshold stand deviation
valid_reading_threshold = 0.7; % percentage require of valid reading in one signature
center_filter = 0.7; % to filter clusters that contains too little readings
base_number = 10;
std_signature = std_sig;
reading_count = 1; % used when generate readings
reading_amount = size(std_signature, 1); % readings size
reading = zeros(reading_amount, size(std_signature, 2)+1);% reading(cluster_id, real_x, real_y, dir, compass_reading, sig)


%% Start point
current_point = [7, 0];

%% Main loop
for mainloop = 1 : reading_amount
    fprintf('round %d\n',mainloop);
    reading(reading_count,1) = 0; % initiate cluster id
    reading(reading_count,2:3) = current_point; % real location
    reference_compass_reading = std_signature(std_signature(:,1) == current_point(1) & std_signature(:,2) == current_point(2), 4);
    [next_point(1) next_point(2)] = get_next_point(current_point);
    reading(reading_count,4) = 2;%direction_convert(compass_reading, reference_compass_reading); % get direction 
    reading(reading_count,5) = reference_compass_reading; % get reference compass reading
    valid = 0;
    reading(reading_count, 6:5+base_number) = std_signature(std_signature(:,1) == current_point(1) & std_signature(:,2) == current_point(2), 5:2:end);
    reading(reading_count, 6+base_number) = 10;
    %% Calculate the believe
    bel_bar = zeros(1,size(center,1)); % initialize the bel_bar
    for j = 1:size(center,1)
        total_count = sum(trans_history(j, reading(reading_count-1,4), :));
        for k = 1:size(center,1)
            if total_count == 0
                trans_p = 0;
            else
                trans_p = trans_history(j, reading(reading_count-1,4), k)/total_count;
            end
            bel_bar(k) = bel_bar(k) + trans_p * bel(j);
        end
    end
    
    %% Get the distance reading and the new believe
    for j = 1:size(center,1) % check all the centers
        %% Calculate euclidean distance between reading and one center
        edist = 0;
        valid_reading_count = 0;
        for sc = 1 : base_number
            if reading(reading_count, 5+sc) ~= inf && center(j, 4+sc) ~= inf
                edist = edist + (reading(reading_count, 5+sc)-center(j, 4+sc)).^2;
                valid_reading_count = valid_reading_count + 1;
            end
        end
        if valid_reading_count < 4  % no such distribution table
            continue;
        end
        edist = edist.^.5;
        %% Calculate the possibility of being in one center
        p = possibility(edist,distribution_table{distribution_table_id}{valid_reading_count});
        bel(j) = p * bel_bar(j);
        %% Record the cluster it belongs to and check whether the threshold need to be changed
        if bel(j) > bel_threshold && (reading(reading_count,1) == 0 || bel(j) > bel(reading(reading_count,1)))
            %% Record
            reading(reading_count,1) = j; % classify the point to one cluster
            %% Check threshold -- Dynamic threshold
            %{
            if mainloop ~= 1
                if j ~= last_cluster
                    distribution_table_id = distr_init;
                    size_remain = 1;
                    size_change = size_change + 1;
                    if size_change > size_change_threshold
                        distribution_table_id = distribution_table_id + 1;
                    end
                else
                    size_change = 0;
                    size_remain = size_remain + 1;
                    if size_remain > size_remain_threshold && distribution_table_id > 1
                        distribution_table_id = distribution_table_id - 1;
                    end
                end
            end
            %}
        end         
    end
    %% Clustering the new reading and generate new center if needed
    if reading(reading_count,1) ~= 0 % find the cluster center that the reading belongs
        center(reading(reading_count, 1), 2) = center(reading(reading_count, 1), 2) + 1;
    else % add the new center to the count -- ctr: cluster_id, contain_reading_number, real_x, real_y, sig1, sig2, sig3 ... sigN
        temp = [];
        temp = [temp size(center, 1)+1]; % cluster_id
        temp = [temp 1]; % contain_reading_number
        temp = [temp reading(reading_count, 2:3)]; % real_x, real_y
        temp = [temp reading(reading_count, 6:5+base_number)]; % sig
        temp = [temp reading(reading_count, end)]; % valid reading number
        center = [center; temp];
        reading(reading_count,1) = size(center,1); % cluster_id
        bel(size(center,1)) = 1; % add new element into  believe
        trans_history(1:size(center, 1), 1:direction_number, size(center, 1)) = trans_init_number;%update the trans_history
        trans_history(size(center, 1), 1:direction_number, 1:size(center, 1)) = trans_init_number;%update the trans_history
    end
    %% Record into trans_history
    if mainloop ~= 1
        trans_history(reading(reading_count-1,1), reading(reading_count-1,4), reading(reading_count,1))=trans_history(reading(reading_count-1,1), reading(reading_count-1,4), reading(reading_count,1))+1;
    end
    %% Normalize believe
    bel_total = sum(bel(:));
    bel = bel / bel_total; 
    %% Next reading
    save 'clustering.mat';
    last_cluster = reading(reading_count,1);
    current_point = next_point;
    reading_count = reading_count + 1;
end


%% Filter the centers
cn = 1/size(center,1)*center_filter*reading_amount;
center_new = [];
count_to_id = [];
for cc = 1 : size(center,1)
   if center(cc,2) > cn 
      center_new = [center_new; center(cc,:)];
      count_to_id = [count_to_id, center(cc, 1)]
   else
      rc = 1;
      while rc <= size(reading, 1)
      	if reading(rc, 1) == center(cc, 1)
            reading(rc, :) = [];
        else
            rc = rc + 1;
        end
      end
   end
end
center_sig = center_new(:,5:end-1);


%% Draw Cluster
colors = [10, 55, 34, 76, 15, 40, 20, 60, 80, 53, 20, 68, 44, 5, 37, 90, 3, 50, 30, 67, 48, 27, 58 ];
scatter(reading(:,2),reading(:,3),75,colors(reading(:,1)),'filled');
axis equal;
axis tight;

%% Clear up 
clear reading_amount;
clear reading_count;
clear mainloop;
clear start_point;
clear random_start_point;
clear current_point;
clear next_point;
clear j;
clear bel_total;
clear temp;
clear colors;
clear cc;
clear rc;
clear cr;
%% Save 
save 'clustering.mat';