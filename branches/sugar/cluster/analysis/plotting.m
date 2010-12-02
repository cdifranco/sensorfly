basic_time = [];
basic_accuracy = [];
basic_bumping = [];
kmeans_time = [];
kmeans_accuracy = [];
kmeans_bumping = [];
random_time = [];
random_accuracy = [];

for i = 1:size(time,2)
    if mod(i,2)==1 
        basic_time = [basic_time time(i)];
        basic_accuracy = [basic_accuracy accuracy(i)];
        basic_bumping = [basic_bumping bumping_record(i)];
    else
        kmeans_time = [kmeans_time time(i)];
        kmeans_accuracy = [kmeans_accuracy accuracy(i)];
        kmeans_bumping = [kmeans_bumping bumping_record(i)];
    end
end
%{
plot(base_number_s:base_number_s-1+size(time,2)/2,basic_time,'r',base_number_s:base_number_s-1+size(time,2)/2,kmeans_time,'b',base_number_s:base_number_s-1+size(time,2)/2,random_time,'g');
figure;
plot(base_number_s:base_number_s-1+size(time,2)/2,basic_accuracy,'r',base_number_s:base_number_s-1+size(time,2)/2,kmeans_accuracy,'b',base_number_s:base_number_s-1+size(time,2)/2,random_accuracy,'g');
%}
plot(base_number_s:base_number_s-1+size(time,2)/2,basic_time,'r',base_number_s:base_number_s-1+size(time,2)/2,kmeans_time,'b');
figure;
plot(base_number_s:base_number_s-1+size(time,2)/2,basic_accuracy,'r',base_number_s:base_number_s-1+size(time,2)/2,kmeans_accuracy,'b');
figure;
plot(base_number_s:base_number_s-1+size(time,2)/2,basic_bumping,'r',base_number_s:base_number_s-1+size(time,2)/2,kmeans_bumping,'b');