initialization;
%%
for main_loop_count = 10:10
    for base_number = 10:10
        fprintf('-----start: base station number %d clustering-----\n', base_number);
        basic_cluster;
  %      fprintf('-----start: base station number %d testing1-----\n', base_number);
  %      testing;

        fprintf('-----start: base station number %d reclustering-----\n', base_number);
        recluster_kmeans;
   %     fprintf('-----start: base station number %d testing2-----\n', base_number);
   %     testing;
        clear trans_history;
        clear trans_history_new;
        clear recluster_table;
        clear temp_metrix;
        clear reading;
 %       fprintf('-----start: base station number %d testing3-----\n', base_number);
 %       rand_testing;
    end
    clear_up;
end

    
