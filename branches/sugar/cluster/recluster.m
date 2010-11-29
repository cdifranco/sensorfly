    recluster_table = zeros(size(center,1),size(center,1));
    for i = 1 : size(center,1)
        for j = 1 : size(center,1)
            recluster_table(i, j) = size(find(reading(:,1) == j & reading(:,end) == i),1)/size(find(reading(:,1) == j),1);
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