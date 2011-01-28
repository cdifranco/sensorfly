% Select 3000 Readings for Clustering
%% Initialization
load ('processed_data.mat');
load ('./loading_files/distribution_table_0p7.mat');
direction_number = 4; % how many direction can each sensorfly take
trans_init_number = 1;
center = []; % ctr: cluster_id, contain_reading_number, real_x, real_y, sig1, sig2, sig3 ... sigN
trans_history = [];     
bel = [];
dir = 0;
base_number = 10;
reading_count = 1;
reading_amount = 3000;
reading = zeros(3000, size(signatures, 2)+1);% reading(cluster_id, real_x, real_y, dir, compass_reading, sig)

%% Start point
random_start_point = unidrnd(size(signatures, 1));
start_point = [signatures(random_start_point, 1) , signatures(random_start_point, 2)];
current_point = start_point;

%% Main loop
for mainloop = 1 : reading_amount
    fprintf('round %d\n',mainloop);
    reading(sig_count,1) = 0; % initiate cluster id
    reading(sig_count,2:3) = current_point; % real location
    [next_reading compass_reading] = next_point(current_point, signature);
    reading(sig_count,4) = direction_covert(compass_reading);
    reading(sig_count,5) = signatures(signatures(:,2:3) == current_point, 4); % get compass reading
    reading(sig_count,6:end) = signatures(signatures(:,2:3) == current_point, 5:end); % get signature
    %% Calculate the believe
    bel_bar = zeros(1,size(center,1)); % initialize the bel_bar
    for j = 1:size(center,1)
        total_count = sum(trans_history(j, reading(sig_count-1,2), :));
        for k = 1:size(center,1)
            if total_count == 0
                trans_p = 0;
            else
                trans_p = trans_history(j, reading(sig_count-1,2), k)/total_count;
            end
            bel_bar(k) = bel_bar(k) + trans_p * bel(j);
        end
    end
    %% Get the distance reading and the new believe
    for j = 1:size(center,1) % check all the centers
        edist = sum((reading(sig_count,6:end)-center(j,5:end)).^2).^.5;
        p = possibility(edist,distribution_table{base_number});
        bel(j) = p * bel_bar(j);
        if bel(j) > bel_threshold && (reading(sig_count,1) == 0 || bel(j) > bel(reading(sig_count,1)))
            reading(sig_count,1) = j;
        end         
    end
    %% Clustering the new reading and generate new center if needed
    if reading(sig_count,1) ~= 0 % find the cluster center that the reading belongs
        center(reading(sig_count,1),2) = center(reading(sig_count,1),2) + 1;
    else % add the new center to the count -- ctr: cluster_id, contain_reading_number, real_x, real_y, sig1, sig2, sig3 ... sigN
        temp = [];
        temp = [temp size(center,1)+1]; % cluster_id
        temp = [temp 1]; % contain_reading_number
        temp = [temp reading(sig_count,2:3)]; % real_x, real_y
        temp = [temp reading(sig_count,6:end)]; % sig
        center = [center; temp];
        reading(sig_count,1) = size(center,1); % cluster_id
        bel(size(center,1)) = 1; % add new element into  believe
        trans_history(1:size(center, 1), 1:direction_number, size(center, 1)) = trans_init_number;%update the trans_history
        trans_history(size(center, 1), 1:direction_number, 1:size(center, 1)) = trans_init_number;%update the trans_history
    end
    %% Record into trans_history
    if mainloop ~= 1
        trans_history(reading(sig_count-1,1), reading(sig_count-1,2), reading(sig_count,1))=trans_history(reading(sig_count-1,1), reading(sig_count-1,2), reading(sig_count,1))+1;
    end
    %% Normalize believe
    bel_total = sum(bel(:));
    bel = bel / bel_total; 
    %% Next reading
    sig_count = sig_count + 1;
    save 'clustering.mat';
    current_point = next_reading;
end