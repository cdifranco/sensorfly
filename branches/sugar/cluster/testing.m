len = 0;
len_o = 0;
error = 0;
error_o = 0;
e = 0;
e_o = 0;
total_bump_count = 0;
total_bump_count_o = 0;
packet_id = 0;

for j = 1:testing_round
    startArea = input('start area: ');
    destArea = input('stop area: ');
    %create matrix
    noOfNodes = size(trans_history,1);
    matrix = zeros(noOfNodes, noOfNodes);
    for l = 1:noOfNodes
        for m = 1:noOfNodes
            sumIJ = sum(trans_history(l,:,m));
            if sumIJ == trans_init_number*direction_number
                matrix(l, m) = inf;
            else
                matrix(l, m) = sum(trans_history(l,:,m))/sum(sum(trans_history(l,:,:)));
                matrix(l, m) = 1/matrix(l, m);
            end;
        end;
    end;
    [succ sigRoute clusterRoute coordRoute startClus destClus] = navigate_basic(packet_id, startArea, destArea, step_len, base_number, trans_history, center_sig, matrix);
    if succ == 1
        len = len + length(clusterRoute)/((sum(([startX, startY]-[destX destY]).^2)).^.5);
        error = error + sum((coordRoute(end,:)-[destX destY]).^2).^.5;
        total_bump_count = total_bump_count + bump_count;
    else 
        e = e + 1;
    end
end
error_rate = error / (testing_round-e);
step_ave = len / (testing_round-e);
error_count = e;
error_rate_o = error_o / (testing_round-e_o);
step_ave_o = len_o / (testing_round-e_o);
error_count_o = e_o;
%measurements
time = [time step_ave step_ave_o]
accuracy = [accuracy error_rate error_rate_o]
%center_count = [center_count size(center, 1)]
error_time = [error_time e e_o]
bumping_record = [bumping_record total_bump_count total_bump_count_o]
