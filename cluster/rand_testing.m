rand_l = 0;
rand_error = 0;
rand_e = 0;
rand_testing_round = 1000;
for j = 1:rand_testing_round
    %percentage = j

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

    %{
    startX = 0;
    startY = 0;
    destX = 3;
    destY = 9;
    %}
    [rand_succ rand_sigRoute rand_clusterRoute rand_coordRoute rand_startClus rand_destClus] = rand_go([startX, startY], [destX, destY], step_len, b, base_number, direction_number, center_sig, room, coefficient);
    if rand_succ == 1
        rand_l = rand_l + length(rand_clusterRoute)/sum(([startX, startY]-[destX destY]).^2).^.5;
        rand_error = rand_error + sum((rand_coordRoute(end,:)-[destX destY]).^2).^.5;
    else 
        rand_e = rand_e + 1;
    end
end
rand_error_rate = rand_error / (rand_testing_round-rand_e);
rand_step_ave = rand_l / (rand_testing_round-rand_e);
rand_error_count = e;
time = [time rand_step_ave]
accuracy = [accuracy rand_error_rate]
error_time = [error_time e]