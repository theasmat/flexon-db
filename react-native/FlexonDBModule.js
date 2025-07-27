import { NativeModules } from 'react-native';
import RNFS from 'react-native-fs';

const { FlexonDBBridge } = NativeModules;

export default {
  createDatabase: async (path, schema) => {
    // Ensure platform-correct file path
    const correctPath = RNFS.DocumentDirectoryPath + '/' + path;
    return FlexonDBBridge.createDatabase(correctPath, schema);
  },
  insertData: async (path, json) => {
    const correctPath = RNFS.DocumentDirectoryPath + '/' + path;
    return FlexonDBBridge.insertData(correctPath, json);
  },
  readData: async (path) => {
    const correctPath = RNFS.DocumentDirectoryPath + '/' + path;
    return FlexonDBBridge.readData(correctPath);
  },
  deleteDatabase: async (path) => {
    const correctPath = RNFS.DocumentDirectoryPath + '/' + path;
    return FlexonDBBridge.deleteDatabase(correctPath);
  },
  updateDatabase: async (path, json) => {
    const correctPath = RNFS.DocumentDirectoryPath + '/' + path;
    return FlexonDBBridge.updateDatabase(correctPath, json);
  },
  csvToFlexonDB: async (csvPath, dbPath) => {
    const correctCsvPath = RNFS.DocumentDirectoryPath + '/' + csvPath;
    const correctDbPath = RNFS.DocumentDirectoryPath + '/' + dbPath;
    return FlexonDBBridge.csvToFlexonDB(correctCsvPath, correctDbPath);
  }
};