basic_time = [];
basic_accuracy = [];
kmeans_time = [];
kmeans_accuracy = [];

for i = 1:size(time,2)
    if mod(i,3)==1 
        basic_time = [basic_time time(i)];
        basic_accuracy = [basic_accuracy accuracy(i)];
    elseif mod(i,3)==2 
        kmeans_time = [kmeans_time time(i)];
        kmeans_accuracy = [kmeans_accuracy accuracy(i)];
    end
end

plot(base_number_s:base_number_s-1+size(time,2)/3,basic_time,'r',base_number_s:base_number_s-1+size(time,2)/3,kmeans_time,'b');
figure;
plot(base_number_s:base_number_s-1+size(time,2)/3,basic_accuracy,'r',base_number_s:base_number_s-1+size(time,2)/3,kmeans_accuracy,'b');