function [valid signature valid_reading] = valid_sig(std_sig_record, std_threshold, valid_reading_threshold, base_number)
%% Initialization
valid = 0;
valid_reading = 0;
signature = zeros(1, base_number); % initialize one signature
%% Validation    
for b = 1 : base_number
    if std_sig_record(6+(b-1)*2) < std_threshold
        signature(b) = std_sig_record(5+(b-1)*2);
        valid_reading = valid_reading + 1;
    else
        signature(b) = inf;
    end
end
if valid_reading > valid_reading_threshold * base_number
   valid = 1; 
end