for i = 4:10
    distribution_table{i-3} = get_distribution_table(i, 0.7, 0.3);
    fprintf('i = %d completed.\n', i);
end
clear i;