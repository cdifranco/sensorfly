clear all;
load 'clustering.mat';
%% Using kmeans to cluster
opts = statset('Display','final');
reading_end = size(reading,2);
[reading(:,reading_end+1), ctrs] = kmeans(reading(:,6:reading_end), size(center_new,1), 'Distance','city', 'Replicates',20, 'Options',opts);
center_sig = ctrs;
p4_match_area_to_cluster;
save 'kmeans_area_cluster.mat';
%%  Generate the tables
recluster_table = zeros(size(center,1),size(center,1));
for i = 1 : size(center,1)
    for j = 1 : size(center,1)
        recluster_table(i, j) = size(find(reading(:,1) == j & reading(:,end) == i),1)/size(find(reading(:,end) == i),1);
    end
end
%%
trans_history_convert = zeros(size(center,1), direction_number, size(center,1));
for i = 1 : direction_number
    for j = 1 : size(center,1)
        for k = 1 : size(center,1)
           temp_matrix(1:size(center,1),1:size(center,1)) = trans_history(:,i,:);
           trans_history_convert(j,i,k)= recluster_table(j,:)* temp_matrix* recluster_table(k,:)';    
        end
    end
end

colors = randperm(size(center,1));
scatter(reading(:,2),reading(:,3),75,colors(reading(:,end)),'filled');
clear colors;
clear i;
clear j;
clear k;
clear temp_matrix;
save 'kmeans_convert.mat';
%%
clear all;
load 'kmeans_area_cluster.mat';
trans_history_new = ones(size(center_new,1),direction_number,size(center_new,1))*trans_init_number;%update the trans_history
for mainloop = 1 : size(reading,2)
    if mainloop ~= 1
        trans_history_new(reading(mainloop-1,end), reading(mainloop-1,4), reading(mainloop,end))=trans_history_new(reading(mainloop-1,end), reading(mainloop-1,4), reading(mainloop,end))+1;
    end
end
clear mainloop;
save 'kmeans_new.mat';