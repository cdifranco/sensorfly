function [succ sig_route error] = rand_go(current_point, dest_area, base_number, dir_number, center_sig, area_cluster_relation,  signatures, distribution_table)
step_threshold = 500;
succ = 1;
error = 0;
total_count = 0;
current_sig = signatures(signatures(:,1)==current_point(1) & signatures(:,2)==current_point(2), 5:2:end);
total_count = total_count + 1;
sig_route(total_count, 1:base_number) = current_sig;
current_cluster = get_cluster_sig(center_sig, current_sig, base_number, distribution_table);
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
current_area = get_area_id(current_point);
fprintf('start from cluster : %d to cluster : %d\n',current_cluster, dest_cluster(1));
fprintf('start from area : %d to area : %d\n',current_area, dest_area);
fprintf('a: %d',current_area);

while 1
    if  ~isempty(find(ismember(dest_cluster, current_cluster), 1))
        fprintf('u r in the destination area %d\n', dest_area);
        current_area = get_area_id(current_point);
        dest_location = get_area_location(dest_area);
        current_location = get_area_location(current_area);
        error =  sqrt(sum((dest_location(:) - current_location(:)) .^ 2));
        fprintf('the actual area %d\n', current_area);
        break;
    else
        suggest_dir = ceil(dir_number*rand);
        fprintf(' --%d--> ', suggest_dir);
        %% Go based on command
        current_point = next_step(current_point, suggest_dir, signatures);
        current_area = get_area_id(current_point);
        fprintf('a: %d ', current_area);
        current_sig = signatures(signatures(:,1)==current_point(1) & signatures(:,2)==current_point(2), 5:2:end);
        total_count = total_count + 1;
        if total_count > step_threshold
            succ = 0;
            break;
        end
        sig_route(total_count, 1:base_number) = current_sig;
        current_cluster = get_cluster_sig(center_sig, current_sig, base_number, distribution_table);
    end
end