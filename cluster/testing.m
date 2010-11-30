l = 0;
error = 0;
e = 0;
testing_round = 100;
for j = 1:testing_round
    startX = 3*rand;
    if startX <= 1
        startY = 4*rand;
    else
        startY = 9*rand;
    end
    destX = 3*rand;
    if destX <= 1
        destY = 4*rand;
    else
        destY = 9*rand;
    end

    [succ sigRoute clusterRoute coordRoute startClus destClus] = Navigate([startX, startY], [destX, destY], step_len, b, base_number, trans_history, trans_init_number, center, room, coefficient);
    if succ == 1
        l = l + length(clusterRoute)/sum(([startX, startY]-[destX destY]).^2).^.5;
        error = error + sum((coordRoute(end,:)-[destX destY]).^2).^.5;
    else 
        e = e + 1;
    end
end
error_rate = error / (testing_round-e);
step_ave = l / (testing_round-e);
%error_count = e;
%measurements
time = [time step_ave]
accuracy = [accuracy error_rate]
%center_count = [center_count size(center, 1)]
%error_time = [error_time e]
