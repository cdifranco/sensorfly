function [succ sigRoute] = navigate_basic(packet_id, port, reading, destArea, base_number, transHistory, center_sig, count_to_id, area_cluster_relation, matrix)
%%
succ = 1;
totalCnt = 0;
directionNumber = size(center_sig, 2);
fprintf('read current signature');
[currentSig packet_id] = get_sig_from_port(packet_id, port, base_number);
totalCnt = totalCnt + 1;
sigRoute(totalCnt, 1:base_number) = currentSig;
currentCluster = get_cluster_sig(center_sig, currentSig)
%destCluster = get_cluster_area(reading, destArea, size(center_sig,1), count_to_id)
%destCluster = area_cluster_relation(destArea);
%%
while 1
    if  currentCluster == destCluster
        fprintf('u r in the dest area %d\n', destArea);
        break;
    else
        [path direction_order direction_count] = guide(currentCluster, destCluster, transHistory, count_to_id, matrix);
        fprintf('go: ');
        if isempty(path)
            fprintf(' random pick--> ');
            unidrnd(directionNumber)
        else
            direction_order
            direction_count
        end
        fprintf('\n');
        stillcontinue = input('continue?(yes:1/no:0)');
        if stillcontinue == 0
            break;
        end
        currentSig = get_sig_from_port(packet_id, port, base_number);
        totalCnt = totalCnt + 1;
        sigRoute(totalCnt, 1:base_number) = currentSig;
        currentCluster = get_cluster_sig(center_sig, currentSig);
    end
end