#import <Foundation/Foundation.h>

NS_ASSUME_NONNULL_BEGIN

@interface FlexonDBBridge : NSObject

+ (int)createDatabase:(NSString *)path schema:(NSString *)schema;
+ (int)insertData:(NSString *)path json:(NSString *)json;
+ (NSString *)readData:(NSString *)path;
+ (int)deleteDatabase:(NSString *)path;
+ (int)updateDatabase:(NSString *)path json:(NSString *)json;
+ (int)csvToFlexonDB:(NSString *)csvPath dbPath:(NSString *)dbPath;

@end

NS_ASSUME_NONNULL_END