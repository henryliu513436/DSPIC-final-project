#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#define NUM_SAMPLES 512
#define M 32

int* twocomplement(int val[],int length){ 
    // size_t length = strlen(val);
    int onescomplement[length];
    int* twocomplement=malloc(length*sizeof(int));
    int carry=1;
    // Finding onescomplement in C  
   for(int i=0;i<length;i++)  
   {  
       if(val[i]==0)  
       onescomplement[i]=1;  
       else if(val[i]==1)  
       onescomplement[i]=0;  
   }  
   onescomplement[length]='\0';  

   for(int i=length-1; i>=0; i--)  
    {  
        if(onescomplement[i] == 1 && carry == 1)  
        {  
            twocomplement[i] = 0;  
        }  
        else if(onescomplement[i] == 0 && carry == 1)  
        {  
            twocomplement[i] = 1;  
            carry = 0;  
        }  
        else  
        {  
            twocomplement[i] = onescomplement[i];  
        }  
    }  
twocomplement[length]='\0';  
// printf("\ntwos: ");  
// for(int i=0;i<length;i++)
//     printf("%d",twocomplement[i]);
return twocomplement;
}
//二進制轉回十進制
double BinaryToDecimal(int bi[],int int_precision, int frac_precision,int bi_length){ //length是bi的長度
    double decimal=0;
    int i=int_precision-1; //1 bit為signed bit
    int j=0;
    int calculation[bi_length-1];
    for(int x=0;x<bi_length-1;x++){
        calculation[x]=bi[x+1]; //把除了signed bit以外裝起來
    }
    int* s_complement=twocomplement(calculation,bi_length-1); //預先處理2's

    //mapping到result
    int result[bi_length-1];
    if(bi[0]==1){
        for (int y=0;y<bi_length-1;y++){
            result[y]=*(s_complement+y); 
        }
    }
    else{
        for (int y=0;y<bi_length-1;y++){
            result[y]=calculation[y]; 
        }
    }
    while(j!=int_precision+frac_precision-1){
        decimal+=result[j]*pow(2,i-1);
        i--;
        j++;
    }
    // printf("\n%lf",decimal);
    free(s_complement);
    if(bi[0]==1)
        return -1*decimal;
    else
        return decimal;
}
//side function for floatToFix
int* integerToBinary(int integerPart, int precision) {

    int* complement=malloc(precision*sizeof(int));  // 長度可以自訂

    if (integerPart == 0) {
        // printf("00");
        complement[0]=0;
        return complement;
    }

    int i = 0;
    while (integerPart > 0) {
        complement[i] = integerPart % 2;
        integerPart /= 2;
        i++;
    }

    // for (int j = precision - 1; j >= 0; j--) {
    //     printf("%d", complement[j]);
    // }
    return complement;
}

// 將十進制轉成二進制 把十進制的double丟進來回傳quantize後的值
double floatToFix(float num, int int_precision, int frac_precision) {
    int sign = 0;  // 符號位，0表示正數
    if (num < 0) {
        sign = 1;
        num = -num;  // 處理負數的情況 負變正
    }

    int integerPart = (int)num;            // 提取整數部分
    // printf("%d\n",integerPart);
    float fractionalPart = num - integerPart;  // 提取小數部分
    int total=int_precision+frac_precision;
    int fix_reference=frac_precision; //作為給binary to decimal的參考值

    int binary[total]; //整數部分跟小數部分算完後加起來s

    int* intpart=integerToBinary(integerPart,int_precision); //integer part pointer
    int a=0;
    int frac[frac_precision]; //把小數部分裝在另一個array
    while (frac_precision > 0) {
        fractionalPart *= 2;
        // printf("%d", (int)fractionalPart);
        frac[a]=(int)fractionalPart;
        fractionalPart -= (int)fractionalPart;
        frac_precision--;
        a++;
    }

    //把整數跟小數array mapping到binary array
    for(int i=0;i<total;i++)
    {
            if(i<int_precision)  //前面幾個bit是整數部分
                binary[i]=*(intpart+int_precision-1-i);
            else
                binary[i]=frac[i-int_precision];
    }

    int length=sizeof(binary)/sizeof(binary[0]); //給2's complement運算用

    int* result=twocomplement(binary, length);
    int final_bi[total+1]; //最後再塞一個signed bit
    if(sign==1)
    {
        // printf("\nreturn output:");
        for(int j=0;j<total;j++)
        {  
            binary[j]=*(result+j);
        } 
    }
    //else output為原來的binary
    free(result);
    free(intpart);
    double answer=BinaryToDecimal(binary, int_precision, fix_reference, length);

    return answer;

}

double generateRandomFloat() {

    int randomInt = rand();
    
    // floating from -1 to 1
    double randomFloat = ((double)randomInt /RAND_MAX) ; //RAND_MAX

    // generate signed numbers
    return (rand() % 2 == 0) ? randomFloat : -randomFloat;
}

double SNR(double fixed[], double floating[]) //fix的output跟floating的output
{
    double SNR=0;
    double Pfloat=0;
    double Pdenominator=0;
    for(int i=0; i<NUM_SAMPLES; i++)
    {
        Pfloat+=floating[i]*floating[i];
    }
    for(int j=0; j<NUM_SAMPLES; j++)
    {
        Pdenominator+=pow((floating[j]-fixed[j]),2);
    }
    SNR=10*log10(Pfloat/Pdenominator);
    return SNR;
}

void firLowPassFilter(double input[], double output[], double filterCoeff[]);
void quatized_firLowPassFilter(double input[], double fix_output[], int wordlength,int MAC_wordlength, double filterCoeff[]);

int main(){
    double inputSignal[NUM_SAMPLES]; // original input
    double outputSignal[NUM_SAMPLES]; // original output
    double fix_out[NUM_SAMPLES]; //計算quantze output
    double filterCoeff[M] = {0.00292513057805888, 0.00943267816539412,-0.00366138523865189, -0.0108829914090833, -0.00138829055779311, 0.0167071159766211, 0.0100076611445859, -0.0203912504328697, -0.0249329349161142, 0.0189512668615024, 0.0479363147628795, -0.00641608420879050, -0.0854618581167195, -0.0357853388990127, 0.185252428888103, 0.403304931808473, 0.403304931808473, 0.185252428888103, -0.0357853388990127, -0.0854618581167195, -0.00641608420879050, 0.0479363147628795, 0.0189512668615024, -0.0249329349161142, -0.0203912504328697, 0.0100076611445859, 0.0167071159766211, -0.00138829055779311, -0.0108829914090833, -0.00366138523865189, 0.00943267816539412, 0.00292513057805888};
    double randomSignal[NUM_SAMPLES];
    double SigtoNoise[50]; //先比前50筆wordlength
    double SigtoNoise1[80]; //比較MAC wordlength
    double first_quantinput[NUM_SAMPLES]; //first quantization 
    
    //original input
    FILE *fptr1;
    fptr1 = fopen("original_input.txt","w");
    //first quantize
    FILE *fptr2;
    fptr2 = fopen("first_quantize_input.txt","w");
    //original output
    FILE *outptr;
    outptr = fopen("original_output.txt","w");
    //fixed output
    FILE *outptr1;
    outptr1 = fopen("quantize_output.txt","w");
    //snr for wordlength change
    FILE *snr1;
    snr1 = fopen("SNR for wordlength change.txt","w");
    //snr for MAC wordlength change
    FILE *snr2;
    snr2 = fopen("SNR for MAC wordlength change.txt","w");
    //wordlength x軸
    FILE *wordlen;
    wordlen = fopen("wordlength.txt","w");
    //wordlength x軸
    FILE *wordlen1;
    wordlen1 = fopen("wordlength 1.txt","w");



    //set random seed
    srand(time(NULL));
    srand((unsigned int)time(NULL));

    double sumOfSquares = 0;

    for (int i = 0; i < NUM_SAMPLES; i++) {
        randomSignal[i] = generateRandomFloat();
        sumOfSquares += randomSignal[i] * randomSignal[i];
    }

    // scale factor to keep average power as 1 
    double scaleFactor = 1 / sqrt(sumOfSquares);

    for (int i = 0; i < NUM_SAMPLES; i++) {
        randomSignal[i] *= scaleFactor;
        inputSignal[i]=randomSignal[i]*sqrt(NUM_SAMPLES); //further scale process
        // printf("inputsignal = %f\n", randomSignal[i]);
        first_quantinput[i]=floatToFix(inputSignal[i],3,12); //先做一次quantiaze讓精度失準
    }

    firLowPassFilter(inputSignal, outputSignal, filterCoeff);

    double averagePower = 0;
    for (int i = 0; i < NUM_SAMPLES; i++) {
        averagePower += inputSignal[i] * inputSignal[i];  //total power
    }
    averagePower /= NUM_SAMPLES; //average power
    printf("Generated signal with average power = %f\n", averagePower);

    printf("Input Signal: ");
    for (int i = 0; i < NUM_SAMPLES; i++) {
        printf("%.10f ", inputSignal[i]);
        fprintf(fptr1,"%.10f\n",inputSignal[i]);
        fprintf(fptr2,"%.10f\n",first_quantinput[i]); //寫出
    }
        //程式結束前閉檔
        fclose(fptr1);
        fclose(fptr2);


    printf("\nOutput Signal (FIR Low Pass Filtered): ");
    for (int i = 0; i < NUM_SAMPLES; i++) {
        printf("%f ", outputSignal[i]);
        fprintf(outptr,"%.10f\n",outputSignal[i]);
    }
        fclose(outptr);
    // printf("%f ", outputSignal[0]);

    //比較inupt wordlength
    for(int i=0;i<50;i++){
        quatized_firLowPassFilter(first_quantinput, fix_out ,i ,80, filterCoeff); //output變
        // printf("%f\n",fix_out[i]);
        SigtoNoise[i]=SNR(fix_out,outputSignal);
        fprintf(snr1,"%.10f\n",SigtoNoise[i]); //寫出
        fprintf(wordlen,"%d\n",i); //寫出
    }
        fclose(snr1);
        fclose(wordlen);
    for(int i=0;i<NUM_SAMPLES;i++)
        fix_out[i]=0.0;

    //比較MAC wordlength
    for(int i=0;i<80;i++){
        quatized_firLowPassFilter(inputSignal, fix_out ,18 ,i, filterCoeff); //讓intput wordlength固定在20
        SigtoNoise1[i]=SNR(fix_out,outputSignal);
        fprintf(snr2,"%.10f\n",SigtoNoise1[i]); //寫出
        fprintf(wordlen1,"%d\n",i); //寫出
    }
        fclose(snr2);
        fclose(wordlen1);


    //把input跟coefficient存成verilog計算形式
    FILE *quanIn;
    quanIn = fopen("input for verilog.txt","w");
    //存coefficient
    FILE *quanCoe;
    quanCoe = fopen("coefficient for verilog.txt","w");
    int quan_input[NUM_SAMPLES];
    int quan_coeff[M];
    for(int i=0;i<NUM_SAMPLES;i++){
        inputSignal[i]=inputSignal[i]*pow(2,18);
        quan_input[i]=floor(inputSignal[i]);
        fprintf(quanIn,"%d\n",quan_input[i]);
    }
    for(int i=0;i<M;i++){
        filterCoeff[i]=filterCoeff[i]*pow(2,18);
        quan_coeff[i]=floor(filterCoeff[i]);
        fprintf(quanCoe,"%d\n",quan_coeff[i]);
    }
    fclose(quanIn);
    fclose(quanCoe);

    return 0;
}

// implementation of original fir filter
void firLowPassFilter(double input[], double output[], double filterCoeff[]) {
    for (int n = 0; n < NUM_SAMPLES; n++) {
        output[n] = 0.0;
        for (int k = 0; k < M; k++) {
            if (n - k >= 0) {
                output[n] += filterCoeff[k] * input[n - k]; //pure convolution
            }
        }
    }
}
// quantization fir filter
void quatized_firLowPassFilter(double input[], double fix_output[], int wordlength,int MAC_wordlength, double filterCoeff[]) { //input是first quantize
//MAC wordlength 是每個tap input跟coefficient相乘的wordleength限制
    double second_quantizeIn[NUM_SAMPLES]; 
    double quantizeCoeff[M];
    double output_beforequantize[NUM_SAMPLES];
    double inbuffer[M];
    double accumulator;
    double out_temp;
    double out; //each element in quantize output
    for(int i=0;i<M;i++)
        inbuffer[i]=0.0; //initialization

    for(int i=0;i<NUM_SAMPLES; i++){
        second_quantizeIn[i]=floatToFix(input[i],3,wordlength); //再做第二次quantize
    }
    for(int i=0;i<M; i++){
        quantizeCoeff[i]=floatToFix(filterCoeff[i],3,wordlength); //coefficinet quantization
    }
    for (int n = 0; n < NUM_SAMPLES; n++) {
        output_beforequantize[n] = 0.0;
        fix_output[n]=0.0;
        out=0.0;
        accumulator=0.0;
        out_temp=0.0;
        for(int i=M-1;i>=0;i--){
            inbuffer[i]=inbuffer[i-1]; //模擬時序
        }
        inbuffer[0]=second_quantizeIn[n];
        for (int k = 0; k < M; k++) {
            accumulator=quantizeCoeff[k]*inbuffer[k];
            out_temp=floatToFix(accumulator,3,MAC_wordlength);
            out+=out_temp;
        }
        fix_output[n]=out;
    }
}

