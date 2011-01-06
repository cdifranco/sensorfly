initialization;
%%
for main_loop_count = 100:100
    for base_number = base_number_s: base_number_e
        %testing paths
        %generate_paths;
        
        %fprintf('-----start: base station number %d clustering-----\n', base_number);
        basic_cluster;
        %{
        fprintf('-----start: base station number %d testing1-----\n', base_number);
        testing;
      
        fprintf('-----start: base station number %d reclustering-----\n', base_number);
        recluster_kmeans;
        fprintf('-----start: base station number %d testing2-----\n', base_number);
        testing;
        clear trans_history;
        clear trans_history_new;
        clear obstacle_history;
        clear obstacle_history_new;
        clear recluster_table;
        clear temp_matrix;
        clear reading;
        fprintf('-----start: base station number %d testing3-----\n', base_number);
        rand_testing;
        %}

    end
    clear_up;
end

    
