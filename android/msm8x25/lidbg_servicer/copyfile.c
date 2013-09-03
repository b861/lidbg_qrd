//功能：复制文件，source：源文件 target 目标文件
void copyfile(char *source, char *target)
{
    FILE *fp;
    FILE *fw; //复制到文件
    char buffer[10];//每次读取10个字符
    if((fp = fopen(source, "rb")) != NULL && (fw = fopen(target, "wb+")) != NULL) //注意这里的打开方式，rb 和 wb 为打开二进制。如果是W 和 R的话，就是以文本形式打开，如果二进制里有-1这个值，就会提交结束哦
    {
        printf("file open ok\n");
        while(!feof(fp))//打开源文件
        {
            fread(buffer, sizeof(char) * 10, 1, fp);
            fwrite(buffer, sizeof(char) * 10, 1, fw);
        }
        //将目标文件的指针定位回文件的起始位置
        rewind(fw);
        fclose(fp);
        fclose(fw);
        printf("file copy ok\n");
    }
    else
    {
        printf("file open err\n");
    }
}
