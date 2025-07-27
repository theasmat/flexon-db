#import "FlexonDBBridge.h"
#import "../../core/flexondb.h"

@implementation FlexonDBBridge

+ (int)createDatabase:(NSString *)path schema:(NSString *)schema {
    return createDatabase([path UTF8String], [schema UTF8String]);
}

+ (int)insertData:(NSString *)path json:(NSString *)json {
    return insertData([path UTF8String], [json UTF8String]);
}

+ (NSString *)readData:(NSString *)path {
    char *result = readData([path UTF8String]);
    NSString *resultStr = [NSString stringWithUTF8String:result];
    free(result);
    return resultStr;
}

+ (int)deleteDatabase:(NSString *)path {
    return deleteDatabase([path UTF8String]);
}

+ (int)updateDatabase:(NSString *)path json:(NSString *)json {
    return updateDatabase([path UTF8String], [json UTF8String]);
}

+ (int)csvToFlexonDB:(NSString *)csvPath dbPath:(NSString *)dbPath {
    return csvToFlexonDB([csvPath UTF8String], [dbPath UTF8String]);
}

@end