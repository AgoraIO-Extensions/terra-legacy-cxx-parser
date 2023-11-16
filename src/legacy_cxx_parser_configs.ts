import { resolvePath } from "@agoraio-extensions/terra-core";
import { ParseFilesConfig } from "@agoraio-extensions/cxx-parser";
import { globSync } from "glob";

export interface LegacyCXXParserConfigs {
  language: string;
  nativeSdkVersion: string;
  includeHeaderDirs: string[];
  definesMacros: string[];
  parseFiles: ParseFilesConfig;
  customHeaders: string[];
  legacyFlags: string[];
  legacyRenders: string[];
}

export class LegacyCXXParserConfigs {
  static resolve(
    configDir: any,
    original: LegacyCXXParserConfigs
  ): LegacyCXXParserConfigs {
    return {
      language: original.language,
      nativeSdkVersion: original.nativeSdkVersion,
      includeHeaderDirs: original.includeHeaderDirs
        .map((it) => {
          // The files order will descend when using globSync. So we sort the files to ensure the order.
          return globSync(resolvePath(it, configDir)).sort();
        })
        .flat(1),
      definesMacros: original.definesMacros ?? [],
      parseFiles: {
        include: (original.parseFiles.include ?? [])
          .map((it) => {
            // The files order will descend when using globSync. So we sort the files to ensure the order.
            return globSync(resolvePath(it, configDir)).sort();
          })
          .flat(1),
        exclude: (original.parseFiles.exclude ?? [])
          .map((it) => {
            // The files order will descend when using globSync. So we sort the files to ensure the order.
            return globSync(resolvePath(it, configDir)).sort();
          })
          .flat(1),
      },
      customHeaders: original.customHeaders
        .map((it) => {
          // The files order will descend when using globSync. So we sort the files to ensure the order.
          return globSync(resolvePath(it, configDir)).sort();
        })
        .flat(1),
      legacyFlags: original.legacyFlags,
      legacyRenders: original.legacyRenders,
    };
  }
}
