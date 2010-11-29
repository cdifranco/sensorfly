% interpolation base data
close all
clear all
load ('distribution')

base_number = 4;%change
main_loop_count = 10;
path_reading_count = 20;
step_len = 0.7;
direction_number = 8;
trans_init_number = 1;
bel_threshold = 0.0;

time = [];
accuracy = [];
center_count = [];
error_time = [];
for base_number = 4:4
    fprintf('-----start: base station number %d clustering-----\n', base_number);
    center = []; %cluster_id, real_x, real_y,sig1,sig2,sig3,sig4
    sig_count = 1;
    %base location
    b = [];
    for i = 1 : base_number
        rand_y = rand*9;
        if rand_y > 4
            rand_x = 1+rand*2;
        else
            rand_x = rand*3;
        end
        b = [b; i rand_x, rand_y];%real location
    end
    trans_history = [];
            
    for mainloop = 1 : main_loop_count
        %initiate the believe vector
        bel = [];
        bel(1:size(center,1)) = 1/size(center,1);
        %generate path and simulate the flight
        for i = 1 : path_reading_count
            % initialization of structure for reading
            reading(sig_count,1) = 0;
            reading(sig_count,2) = ceil(rand*direction_number);
            if i == 1
                reading(sig_count, 3) = 3;
                reading(sig_count, 4) = 7+rand;
            else
                reading(sig_count, 3:4) = generate_next_step(direction_number, reading(sig_count-1,2), step_len, reading(sig_count-1,3), reading(sig_count-1,4));
            end
            reading(sig_count, 5:4+base_number) = convert(reading(sig_count,3), reading(sig_count,4), base_number, b);
            % initialize the bel_bar
            bel_bar = zeros(1,size(center,1));
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
            % get the distance reading and the new bel
            for j = 1:size(center,1) %check all the centers
                edist = (sum((reading(sig_count,5:end)-center(j,4:end)).^2)).^.5;
                p = possibility(edist,distributionTable);
                bel(j) = p * bel_bar(j);
                if bel(j) > bel_threshold & (reading(sig_count,1) == 0 | bel(j) > bel(reading(sig_count,1)))
                    reading(sig_count,1) = j;
                end         
            end
            if reading(sig_count,1) == 0
                % add the new center to the count
                temp = [];
                temp = [temp size(center,1)+1];
                temp = [temp reading(sig_count,3)];
                temp = [temp reading(sig_count,4)];
                temp_sig = convert(temp(2),temp(3),base_number,b);
                temp = [temp temp_sig];
                center = [center; temp];
                reading(sig_count,1) = size(center,1);
                bel(size(center,1)) = 1;
                %update the trans_history
                trans_history(1:size(center, 1), 1:direction_number, size(center, 1)) = trans_init_number;
                trans_history(size(center, 1), 1:direction_number, 1:size(center, 1)) = trans_init_number;
            end
            if i ~= 1
                trans_history(reading(sig_count-1,1), reading(sig_count-1,2), reading(sig_count,1))=trans_history(reading(sig_count-1,1), reading(sig_count-1,2), reading(sig_count,1))+1;
            end
            %normalize the bel
            bel_total =sum(bel(:));
            bel = bel / bel_total;
            sig_count = sig_count + 1;
        end
    end
    
    draw_cluster(1,size(center,1),reading);
    fprintf('-----start: base station number %d testing1-----\n', base_number);
    testing;
    time = [time step_ave]
    accuracy = [accuracy error_rate]
    center_count = [center_count size(center, 1)]
    error_time = [error_time e]

%--------------------------------------------------------------------------    
    fprintf('-----start: base station number %d reclustering-----\n', base_number);
    % using kmeans to cluster while generate the trans_possibility_table\
    opts = statset('Display','final');
    [reading(:,end+1), ctrs] = kmeans(reading(:,5:end), size(center,1), 'Distance','city', 'Replicates',20, 'Options',opts);
%--------------------------------------------------------------------------
    recluster_table = zeros(size(center,1),size(center,1));
    for i = 1 : size(center,1)
        for j = 1 : size(center,1)
            recluster_table(i, j) = size(find(reading(:,1) == j & reading(:,end) == i),1)/size(find(reading(:,end) == i),1);
        end
    end
    
    trans_history_new = zeros(size(center,1), direction_number, size(center,1));
    for i = 1 : direction_number
        for j = 1 : size(center,1)
            for k = 1 : size(center,1)
               temp_metrix(1:size(center,1),1:size(center,1)) = trans_history(:,i,:);
               trans_history_new(j,i,k)= recluster_table(j,:)* temp_metrix* recluster_table(k,:)';    
            end
        end
    end
    trans_history = trans_history_new;
    figure;
    draw_cluster(size(reading,2),size(center,1),reading);
    fprintf('-----start: base station number %d testing2-----\n', base_number);
    testing;
    time = [time step_ave]
    accuracy = [accuracy error_rate]
    center_count = [center_count size(center, 1)]
    error_time = [error_time e]

end
% clean up
clear ans;
clear bel;
clear bel_bar;
clear bel_threshold;
clear bel_total;
clear edist;
clear grid_x_number;
clear grid_y_number;
clear i;
clear j;
clear k;
clear main_loop_count;
clear main_loop;
clear p;
clear path_reading_count;
clear round1;
clear round2;
clear sig_count;
clear temp;
clear total_count;
clear trans_p;
clear x;
clear y;
%hold off;