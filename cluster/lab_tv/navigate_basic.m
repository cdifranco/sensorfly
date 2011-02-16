function [succ sigRoute] = navigate_basic(packet_id, port, dest_area, base_number, transHistory, center_sig, count_to_id, matrix, area_cluster_relation, grid)
%% Initialization
step_threshold = 500;
succ = 1;
totalCnt = 0;
directionNumber = size(transHistory, 2);
destCluster = [];
direction_pool = ['b','l','f','r'];
fprintf('read current signature');
[currentSig packet_id] = get_sig_from_port(packet_id, port, base_number);
totalCnt = totalCnt + 1;
sigRoute(totalCnt, 1:base_number) = currentSig;
currentCluster = get_cluster_sig(center_sig, currentSig);
%destCluster = get_cluster_area(reading, dest_area, size(center_sig,1), count_to_id)
%% get dest clusters
clusters_in_area = area_cluster_relation{dest_area};
[max_cluster_id max_cluster_number] = mode(clusters_in_area);
if max_cluster_number < 20
    destCluster = clusters_in_area(1:20);
else    
    while max_cluster_number >= 20
        destCluster = [destCluster max_cluster_id];
        clusters_in_area(clusters_in_area(:)==max_cluster_id) = [];
        [max_cluster_id max_cluster_number] = mode(clusters_in_area);  
    end
end
%% Main while loop
while 1
    
    %% Draw on lab picture
    area_id = socket_receive(d_input_stream, 1);
    if area_id == 0 
        break;
    end
    for j =  1 : 2
        surf(grid{area_id}.x, grid{area_id}.y, ones(4)*0, ones(4)*-1);
        axis tight;
        axis equal;
        colormap([1 1 0; 0 1 1])
        pause(1);
        surf(grid{area_id}.x, grid{area_id}.y, ones(4)*0, ones(4)*0);
        axis tight;
        axis equal;
        colormap([1 1 0; 0 1 1])
        pause(1);
    end
    hold on;
    %%
    if  ~isempty(find(ismember(destCluster, currentCluster), 1))
        fprintf('u r in the dest area %d\n', dest_area);
        socket_send(d_output_stream, 's');
        break;
    else
        %%
        [path direction_order] = guide(currentCluster, destCluster(1), transHistory, count_to_id, matrix);
        if isempty(path)
            fprintf('random-->');
            suggest_dir = double(unidrnd(directionNumber));
        else
            suggest_dir = double(direction_order(1));
        end
        %% give out direction through socket 
        socket_send(d_output_stream, direction_pool(suggest_dir));
       
        %%
        currentSig = get_sig_from_port(packet_id, port, base_number);
        totalCnt = totalCnt + 1;
        if totalCnt > step_threshold
            socket_send('e',125,10);
            succ = 0;
            break;
        end
        sigRoute(totalCnt, 1:base_number) = currentSig;
        currentCluster = get_cluster_sig(center_sig, currentSig);
    end
end