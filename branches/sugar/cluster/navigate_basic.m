function [succ sigRoute] = navigate_basic(packet_id, port, reading, destArea, base_number, transHistory, center_sig, count_to_id, matrix, top)
%%
step_threshold = 50;
succ = 1;
totalCnt = 0;
directionNumber = size(transHistory, 2);
fprintf('read current signature');
[currentSig packet_id] = get_sig_from_port(packet_id, port, base_number);
totalCnt = totalCnt + 1;
sigRoute(totalCnt, 1:base_number) = currentSig;
currentCluster = get_cluster_sig(center_sig, currentSig)
destCluster = get_cluster_area(reading, destArea, size(center_sig,1), count_to_id)
%destCluster = area_cluster_relation((destArea-1)*top+1:destArea*top);
%%
while 1
    if  ~isempty(find(ismember(destCluster(1), currentCluster), 1))
        fprintf('u r in the dest area %d\n', destArea);
        break;
    else
        [path direction_order] = guide(currentCluster, destCluster(1), transHistory, count_to_id, matrix);
        fprintf('go: ');
        if isempty(path)
            fprintf(' random pick--> %d\n',uint32(unidrnd(directionNumber)));  
        else
            fprintf(' pick based on map--> %d\n', uint32(direction_order(1)));
        end
        stillcontinue = input('continue?(yes:1/no:0)');
        if stillcontinue == 0
            break;
        end
        currentSig = get_sig_from_port(packet_id, port, base_number);
        totalCnt = totalCnt + 1;
        if totalCnt > step_threshold
            succ = 0;
            break;
        end
        sigRoute(totalCnt, 1:base_number) = currentSig;
        currentCluster = get_cluster_sig(center_sig, currentSig);
    end
end