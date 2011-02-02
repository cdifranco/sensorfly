function area_id = get_area_id(current_point)
if current_point(2) >= 0 && current_point(2) <= 90
    if current_point(1) >= 100 && current_point(1) <= 190
        area_id = 4;
    elseif current_point(1) >= 200 && current_point(1) <= 290
        area_id = 3;
    elseif current_point(1) >= 300 && current_point(1) <= 390
        area_id = 2;
    else
        area_id = 1;
    end
elseif current_point(2) >= 100 && current_point(2) <= 190
    if current_point(1) >= 100 && current_point(1) <= 190
        area_id = 5;
    elseif current_point(1) >= 200 && current_point(1) <= 290
        area_id = 6;
    else
        area_id = 7;
    end
elseif current_point(2) >= 200 && current_point(2) <= 290
    if current_point(1) >= 0 && current_point(1) <= 90
        area_id = 12;
    elseif current_point(1) >= 100 && current_point(1) <= 190
        area_id = 11;
    elseif current_point(1) >= 200 && current_point(1) <= 290
        area_id = 10;
    elseif current_point(1) >= 300 && current_point(1) <= 390
        area_id = 9;
    else
        area_id = 8;
    end
end

