initialization;
%%
for main_loop_count = 1:15
    for base_number = base_number_s: base_number_e
        %% testing paths
	     generate_paths;
        %%
        fprintf('-----start: base station number %d clustering-----\n', base_number);
        basic_cluster;
        save 'r249_i1to10_dis1p0_basic_cluster_bn4to12.mat';
        %%
        fprintf('-----start: base station number %d testing1-----\n', base_number);
        testing;
        save 'r249_i1to10_dis1p0_basic_cluster_testing_bn4to12.mat';
        %%
        fprintf('-----start: base station number %d reclustering-----\n', base_number);
        recluster_kmeans;
        save 'r249_i1to10_dis1p0_kmeans_bn4to12.mat';
        %%
        fprintf('-----start: base station number %d testing2-----\n', base_number);
        testing;
        save 'r249_i1to10_dis1p0_kmeans_testing_bn4to12.mat';
        %%
        clear trans_history;
        clear trans_history_new;
        clear recluster_table;
        clear temp_metrix;
        clear reading;
        %%
        fprintf('-----start: base station number %d testing3-----\n', base_number);
        rand_testing;
    end
    clear_up;
end
save 'r249_i1to10_dis1p0_final_bn4to12.mat';

    
