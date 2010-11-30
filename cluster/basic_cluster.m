fprintf('-----initialization-----\n', base_number);
center = []; %cluster_id, real_x, real_y,sig1,sig2,sig3...sigN
sig_count = 1;
trans_history = [];
generate_base;      
coefficient = polyfit(sigMeanDist(:, 1), sigMeanDist(: ,2), 3);
fprintf('-----main loop-----\n', base_number);
for mainloop = 1 : main_loop_count
    %initiate the believe vector
    bel = [];
    bel(1:size(center,1)) = 1/size(center,1);
    %generate path and simulate the flight
    for i = 1 : path_reading_count
        % initialization of structure for reading
        reading(sig_count,1) = 0;
        reading(sig_count,2) = ceil(rand*direction_number);
        % flying from side door
        if i == 1
            reading(sig_count, 3) = 3 - .00001;
            reading(sig_count, 4) = 7+rand;
        else
            reading(sig_count, 3:4) = generate_next_step(direction_number,direction_number, reading(sig_count-1,2), step_len, reading(sig_count-1,3), reading(sig_count-1,4),room);
        end
        reading(sig_count, 5:4+base_number) = convert(reading(sig_count,3), reading(sig_count,4), base_number, b, coefficient);
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
            p = possibility(edist,distribution_table{base_number-3});
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
            temp_sig = convert(temp(2),temp(3),base_number,b,coefficient);
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
