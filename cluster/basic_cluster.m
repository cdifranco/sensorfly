center = []; %cluster_id, contain_reading_number, real_x, real_y,sig1,sig2,sig3...sigN
sig_count = 1;
trans_history = [];     

for mainloop = 1 : main_loop_count
    node_id = mainloop + 10;
    %initiate the believe vector
    bel = [];
    bel(1:size(center,1)) = 1/size(center,1);
    reading(sig_count,1) = 0;
    reading(sig_count,2) = get_dir_from_port(node_id, port);
    reading(sig_count, 3:4) = get_location();
    reading(sig_count, 5:4+base_number) = get_sig_from_port(node_id, port, base_number);% number of node we are observing
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
        edist = sum((reading(sig_count,5:end)-center(j,5:end)).^2).^.5;
        p = possibility(edist,distribution_table{base_number});
        bel(j) = p * bel_bar(j);
        if bel(j) > bel_threshold && (reading(sig_count,1) == 0 || bel(j) > bel(reading(sig_count,1)))
            reading(sig_count,1) = j;
        end         
    end
    if reading(sig_count,1) ~= 0
        center(reading(sig_count,1),2) = center(reading(sig_count,1),2) + 1;
    else
        % add the new center to the count
        temp = [];
        temp = [temp size(center,1)+1];
        temp = [temp 1];
        temp = [temp reading(sig_count,3:end)];
        center = [center; temp];
        reading(sig_count,1) = size(center,1);
        bel(size(center,1)) = 1;
        %update the trans_history
        trans_history(1:size(center, 1), 1:direction_number, size(center, 1)) = trans_init_number;
        trans_history(size(center, 1), 1:direction_number, 1:size(center, 1)) = trans_init_number;
        obstacle_history(size(center, 1), 1:direction_number) = 0;
    end
    if mainloop ~= 1
        trans_history(reading(sig_count-1,1), reading(sig_count-1,2), reading(sig_count,1))=trans_history(reading(sig_count-1,1), reading(sig_count-1,2), reading(sig_count,1))+1;
    end
    %normalize the bel
    bel_total =sum(bel(:));
    bel = bel / bel_total;
    sig_count = sig_count + 1;
end
%{
figure;
draw_cluster(1,size(center,1),reading);
hold on;
draw_center;
hold off;
%}
center_sig = center(:,5:end);