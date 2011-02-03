% Select 3000 Readings for Clustering
%% Initialization
load ('processed_data.mat');
load ('../loading_files/distribution_table_0p9.mat');
direction_number = 4; % how many direction can each sensorfly take
trans_init_number = 1;
center = []; % ctr: cluster_id, contain_reading_number, real_x, real_y, sig1, sig2, sig3 ... sigN
trans_history = [];     
bel = [];
bel_threshold = 0.0001;
center_filter = 0.7;
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
    reading(reading_count,1) = 0; % initiate cluster id
    reading(reading_count,2:3) = current_point; % real location
    reference_compass_reading = signatures(signatures(:,1) == current_point(1) & signatures(:,2) == current_point(2), 4);
    [next_reading(1) next_reading(2) compass_reading] = next_point(current_point, signatures);
    cr = round(compass_reading);
    reading(reading_count,4) = direction_convert(cr, reference_compass_reading); % get direction 
    reading(reading_count,5) =  reference_compass_reading; % get reference compass reading
    reading(reading_count,6:end) = signatures(signatures(:,1) == current_point(1) & signatures(:,2) == current_point(2), 5:end); % get signature
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
        edist = sum((reading(reading_count,6:end)-center(j,5:end)).^2).^.5;
        p = possibility(edist,distribution_table{base_number});
        bel(j) = p * bel_bar(j);
        if bel(j) > bel_threshold && (reading(reading_count,1) == 0 || bel(j) > bel(reading(reading_count,1)))
            reading(reading_count,1) = j;
        end         
    end
    %% Clustering the new reading and generate new center if needed
    if reading(reading_count,1) ~= 0 % find the cluster center that the reading belongs
        center(reading(reading_count,1),2) = center(reading(reading_count,1),2) + 1;
    else % add the new center to the count -- ctr: cluster_id, contain_reading_number, real_x, real_y, sig1, sig2, sig3 ... sigN
        temp = [];
        temp = [temp size(center,1)+1]; % cluster_id
        temp = [temp 1]; % contain_reading_number
        temp = [temp reading(reading_count,2:3)]; % real_x, real_y
        temp = [temp reading(reading_count,6:end)]; % sig
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
    reading_count = reading_count + 1;
    save 'clustering.mat';
    current_point = next_reading;
end
%filter the centers
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
center_sig = center_new(:,5:end);
%% Draw Cluster
colors = randperm(size(center,1));
scatter(reading(:,2),reading(:,3),75,colors(reading(:,1)),'filled');
%% Clear up 
clear reading_amount;
clear reading_count;
clear mainloop;
clear start_point;
clear random_start_point;
clear current_point;
clear next_reading;
clear j;
clear bel_total;
clear temp;
clear colors;
clear cc;
clear rc;
clear cr;
%% Save 
save 'clustering.mat';