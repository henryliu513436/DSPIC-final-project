clear; close all;

fs = 8000;        % 采样率
f = [1500 2000];    % 截止频率
a = [1 0];        % 期望幅值
 
dev = [0.01 0.01]; %ripple大小
[n,fo,ao,w] = firpmord(f,a,dev,fs);
b = firpm(n,fo,ao,w); %用最優濾波器定義31 tap

figure(1) 
freqz(b,1,1024,fs);
hold on

frange = 0:1:4000;
Mask = zeros(size(frange));
Mask(frange <= 1500) = 0; % Passband
Mask(frange >= 2000) = -40; % Stopband
Mask(frange > 1500 & frange < 2000) = interp1([1500,2000], [0, -40], frange(frange > 1500 & frange < 2000)); 
figure(1)
plot(frange, Mask, 'r--', 'LineWidth', 2);
hold off;
grid on;
xlabel('Frequency (Hz)');
ylabel('Magnitude (dB)');
legend('Frequency Response', 'Frequency Mask');

% original input vs original output
In = fopen('original_input.txt','r');
format = '%f'; %定義資料型態
input = fscanf(In, format);
figure(2),
freqz(input)
title("original input spectrum")
Out = fopen('original_output.txt','r');
output = fscanf(Out, format);
figure(3)
freqz(output)
title("original output spectrum")

%SNR for word length change
SNR=fopen('SNR for wordlength change.txt','r');
SNR1 = fscanf(SNR, format);
wlen = fopen('wordlength.txt','r');
formatINT = '%d';
wordlength = fscanf(wlen, formatINT);
figure(4),
plot(wordlength,SNR1);
title("input word length vs SNR")
xlabel("input word length")
ylabel("SNR1")

%SNR for Mac word length change
SNR=fopen('SNR for MAC wordlength change.txt','r');
SNR2 = fscanf(SNR, format);
wlen1 = fopen('wordlength 1.txt','r');
formatINT = '%d';
wordlength = fscanf(wlen1, formatINT);
figure(5),
plot(wordlength,SNR2);
title("MAC word length vs SNR")
xlabel("MAC word length")
ylabel("SNR2")

