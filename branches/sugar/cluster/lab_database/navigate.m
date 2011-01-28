function [succ sig_route] = navigate(dest_area, base_number, trans_history, center_sig, count_to_id, matrix, area_cluster_relation)
%% Initialization
step_threshold = 500;
succ = 1;
total_count = 0;
dir_number = size(trans_history, 2);
% TODO: get current_sig, start from any point
current_sig = 0;
total_count = total_count + 1;
sig_route(total_count, 1:base_number) = current_sig;
current_cluster = get_cluster_sig(center_sig, current_sig);
dest_cluster = [];
%% Get dest clusters
clusters_in_area = area_cluster_relation{dest_area};
[max_cluster_id max_cluster_number] = mode(clusters_in_area);
if max_cluster_number < 20
    dest_cluster = clusters_in_area(1:20);
else    
    while max_cluster_number >= 20
        dest_cluster = [dest_cluster max_cluster_id];
        clusters_in_area(clusters_in_area(:)==max_cluster_id) = [];
        [max_cluster_id max_cluster_number] = mode(clusters_in_area);  
    end
end
%% Main while loop
while 1
    %%
    if  ~isempty(find(ismember(dest_cluster, current_cluster), 1))
        fprintf('u r in the destination area %d\n', dest_area);
        break;
    else
        %% Get direction
        [path direction_order] = guide(current_cluster, dest_cluster(1), trans_history, count_to_id, matrix);
        if isempty(path)
            fprintf('random pick --> ');
            suggest_dir = double(unidrnd(dir_number));
        else
            suggest_dir = double(direction_order(1));
        end
        fprintf('direction : %d', suggest_dir);
        %% Go based on command
        current_sig = 0; % TODO : add next point
        total_count = total_count + 1;
        if total_count > step_threshold
            succ = 0;
            break;
        end
        sig_route(total_count, 1:base_number) = current_sig;
        current_cluster = get_cluster_sig(center_sig, current_sig);
    end
end