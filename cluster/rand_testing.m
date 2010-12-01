rand_l = 0;
rand_error = 0;
rand_e = 0;

for j = 1:testing_round
    startX = path(j,1);
    startY = path(j,2);
    destX = path(j,3);
    destY = path(j,4);
    [rand_succ rand_sigRoute rand_clusterRoute rand_coordRoute rand_startClus rand_destClus] = rand_go([startX, startY], [destX, destY], step_len, b, base_number, direction_number, center_sig, room, coefficient);
    if rand_succ == 1
        rand_l = rand_l + length(rand_clusterRoute)/sum(([startX, startY]-[destX destY]).^2).^.5;
        rand_error = rand_error + sum((rand_coordRoute(end,:)-[destX destY]).^2).^.5;
    else 
        rand_e = rand_e + 1;
    end
end
rand_error_rate = rand_error / (testing_round-rand_e);
rand_step_ave = rand_l / (testing_round-rand_e);
rand_error_count = e;
time = [time rand_step_ave]
accuracy = [accuracy rand_error_rate]
error_time = [error_time e]