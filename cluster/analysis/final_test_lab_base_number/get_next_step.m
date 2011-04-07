% used for generate next step in testing
function new_coord = get_next_step(direction, current_point, sigxy)
tx = current_point(1);
ty = current_point(2);
angle = angle_convert(direction);
%fprintf('%d, %d\n',direction, direction_convert(angle));
step_len = 4 + unidrnd(1); % step length is from 40cm to 50 cm
new_x = (tx/10 + round(step_len * cos(angle)))*10;
new_y = (ty/10 + round(step_len * sin(angle)))*10;
sentry_distance = inf;
%% project location to grid
for i = tx - step_len*10 :10: tx + step_len*10
    for j = ty - step_len*10 :10: ty + step_len*10
        if isempty(find(sigxy.x== i & sigxy.y == j, 1))
            continue; % check if the point has reading
        else
            distance = sqrt(sum(([new_x, new_y] - [i, j]) .^ 2));
            if distance < sentry_distance
                sentry_distance = distance;
                new_coord = [i, j];
            end
        end
    end
end