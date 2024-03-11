/*
 Licensed to the Apache Software Foundation (ASF) under one
 or more contributor license agreements.  See the NOTICE file
 distributed with this work for additional information
 regarding copyright ownership.  The ASF licenses this file
 to you under the Apache License, Version 2.0 (the
 "License"); you may not use this file except in compliance
 with the License.  You may obtain a copy of the License at

 http://www.apache.org/licenses/LICENSE-2.0

 Unless required by applicable law or agreed to in writing,
 software distributed under the License is distributed on an
 "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
 KIND, either express or implied.  See the License for the
 specific language governing permissions and limitations
 under the License.
 */

#import <Foundation/Foundation.h>
#import <Cordova/CDVPlugin.h>

extern NSString* const kCDVAssetsLibraryPrefix;
extern NSString* const kCDVFsgbFilesystemURLPrefix;

enum CDVFsgbFileError {
    NO_ERROR = 0,
    NOT_FOUND_ERR = 1,
    SECURITY_ERR = 2,
    ABORT_ERR = 3,
    NOT_READABLE_ERR = 4,
    ENCODING_ERR = 5,
    NO_MODIFICATION_ALLOWED_ERR = 6,
    INVALID_STATE_ERR = 7,
    SYNTAX_ERR = 8,
    INVALID_MODIFICATION_ERR = 9,
    QUOTA_EXCEEDED_ERR = 10,
    TYPE_MISMATCH_ERR = 11,
    PATH_EXISTS_ERR = 12
};
typedef int CDVFsgbFileError;

@interface CDVFsgbFilesystemURL : NSObject  {
    NSURL *_url;
    NSString *_fileSystemName;
    NSString *_fullPath;
}

- (id) initWithString:(NSString*)strURL;
- (id) initWithURL:(NSURL*)URL;
+ (CDVFsgbFilesystemURL *)fileSystemURLWithString:(NSString *)strURL;
+ (CDVFsgbFilesystemURL *)fileSystemURLWithURL:(NSURL *)URL;

- (NSString *)absoluteURL;

@property (atomic) NSURL *url;
@property (atomic) NSString *fileSystemName;
@property (atomic) NSString *fullPath;

@end

@interface CDVFsgbFilesystemURLProtocol : NSURLProtocol
@end

@protocol CDVFsgbFileSystem
- (CDVPluginResult *)entryForLocalURI:(CDVFsgbFilesystemURL *)url;
- (CDVPluginResult *)getFileForURL:(CDVFsgbFilesystemURL *)baseURI requestedPath:(NSString *)requestedPath options:(NSDictionary *)options;
- (CDVPluginResult *)getParentForURL:(CDVFsgbFilesystemURL *)localURI;
- (CDVPluginResult *)setMetadataForURL:(CDVFsgbFilesystemURL *)localURI withObject:(NSDictionary *)options;
- (CDVPluginResult *)removeFileAtURL:(CDVFsgbFilesystemURL *)localURI;
- (CDVPluginResult *)recursiveRemoveFileAtURL:(CDVFsgbFilesystemURL *)localURI;
- (CDVPluginResult *)readEntriesAtURL:(CDVFsgbFilesystemURL *)localURI;
- (CDVPluginResult *)truncateFileAtURL:(CDVFsgbFilesystemURL *)localURI atPosition:(unsigned long long)pos;
- (CDVPluginResult *)writeToFileAtURL:(CDVFsgbFilesystemURL *)localURL withData:(NSData*)encData append:(BOOL)shouldAppend;
- (void)copyFileToURL:(CDVFsgbFilesystemURL *)destURL withName:(NSString *)newName fromFileSystem:(NSObject<CDVFsgbFileSystem> *)srcFs atURL:(CDVFsgbFilesystemURL *)srcURL copy:(BOOL)bCopy callback:(void (^)(CDVPluginResult *))callback;
- (void)readFileAtURL:(CDVFsgbFilesystemURL *)localURL start:(NSInteger)start end:(NSInteger)end callback:(void (^)(NSData*, NSString* mimeType, CDVFsgbFileError))callback;
- (void)getFileMetadataForURL:(CDVFsgbFilesystemURL *)localURL callback:(void (^)(CDVPluginResult *))callback;

- (NSDictionary *)makeEntryForLocalURL:(CDVFsgbFilesystemURL *)url;
- (NSDictionary*)makeEntryForPath:(NSString*)fullPath isDirectory:(BOOL)isDir;

@property (nonatomic,strong) NSString *name;
@property (nonatomic, copy) NSURL*(^urlTransformer)(NSURL*);

@optional
- (NSString *)filesystemPathForURL:(CDVFsgbFilesystemURL *)localURI;
- (CDVFsgbFilesystemURL *)URLforFilesystemPath:(NSString *)path;

@end

@interface CDVFsgbFile : CDVPlugin {
    NSString* rootDocsPath;
    NSString* appDocsPath;
    NSString* appLibraryPath;
    NSString* appTempPath;

    NSMutableArray* fileSystems_;
    BOOL userHasAllowed;
}

- (NSNumber*)checkFreeDiskSpace:(NSString*)appPath;
- (NSDictionary*)makeEntryForPath:(NSString*)fullPath fileSystemName:(NSString *)fsName isDirectory:(BOOL)isDir;
- (NSDictionary *)makeEntryForURL:(NSURL *)URL;
- (CDVFsgbFilesystemURL *)fileSystemURLforLocalPath:(NSString *)localPath;

- (NSObject<CDVFsgbFileSystem> *)filesystemForURL:(CDVFsgbFilesystemURL *)localURL;

/* Native Registration API */
- (void)registerFilesystem:(NSObject<CDVFsgbFileSystem> *)fs;
- (NSObject<CDVFsgbFileSystem> *)fileSystemByName:(NSString *)fsName;

/* Exec API */
- (void)requestFileSystem:(CDVInvokedUrlCommand*)command;
- (void)resolveLocalFileSystemURI:(CDVInvokedUrlCommand*)command;
- (void)getDirectory:(CDVInvokedUrlCommand*)command;
- (void)getFile:(CDVInvokedUrlCommand*)command;
- (void)getParent:(CDVInvokedUrlCommand*)command;
- (void)removeRecursively:(CDVInvokedUrlCommand*)command;
- (void)remove:(CDVInvokedUrlCommand*)command;
- (void)copyTo:(CDVInvokedUrlCommand*)command;
- (void)moveTo:(CDVInvokedUrlCommand*)command;
- (void)getFileMetadata:(CDVInvokedUrlCommand*)command;
- (void)readEntries:(CDVInvokedUrlCommand*)command;
- (void)readAsText:(CDVInvokedUrlCommand*)command;
- (void)readAsDataURL:(CDVInvokedUrlCommand*)command;
- (void)readAsArrayBuffer:(CDVInvokedUrlCommand*)command;
- (void)write:(CDVInvokedUrlCommand*)command;
- (void)testFileExists:(CDVInvokedUrlCommand*)command;
- (void)testDirectoryExists:(CDVInvokedUrlCommand*)command;
- (void)getFreeDiskSpace:(CDVInvokedUrlCommand*)command;
- (void)truncate:(CDVInvokedUrlCommand*)command;
- (void)doCopyMove:(CDVInvokedUrlCommand*)command isCopy:(BOOL)bCopy;

/* Compatibilty with older File API */
- (NSString*)getMimeTypeFromPath:(NSString*)fullPath;
- (NSDictionary *)getDirectoryEntry:(NSString *)target isDirectory:(BOOL)bDirRequest;

/* Conversion between filesystem paths and URLs */
- (NSString *)filesystemPathForURL:(CDVFsgbFilesystemURL *)URL;

/* Internal methods for testing */
- (void)_getLocalFilesystemPath:(CDVInvokedUrlCommand*)command;

@property (nonatomic, strong) NSString* rootDocsPath;
@property (nonatomic, strong) NSString* appDocsPath;
@property (nonatomic, strong) NSString* appLibraryPath;
@property (nonatomic, strong) NSString* appTempPath;
@property (nonatomic, strong) NSString* persistentPath;
@property (nonatomic, strong) NSString* temporaryPath;
@property (nonatomic, strong) NSMutableArray* fileSystems;

@property BOOL userHasAllowed;

@end

#define kW3FileTemporary @"temporary"
#define kW3FilePersistent @"persistent"
