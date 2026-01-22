/**
 * Game Data Manager
 * Handles storage, retrieval, and validation of game data files
 */

const DataManager = (function() {
    'use strict';

    // IndexedDB configuration
    const DB_NAME = 'UltimaEnginesDB';
    const DB_VERSION = 1;
    const STORES = {
        gameData: 'gameData',
        config: 'config'
    };

    // Game definitions with required files
    const GAME_DEFINITIONS = {
        u7bg: {
            name: 'Ultima VII: The Black Gate',
            engine: 'exult',
            requiredFiles: [
                'static/mainshp.flx',
                'static/shapes.vga',
                'static/u7map',
                'static/usecode',
                'static/initgame.dat'
            ],
            optionalFiles: [
                'static/faces.vga',
                'static/sprites.vga',
                'static/gumps.vga'
            ]
        },
        u7si: {
            name: 'Ultima VII Part Two: Serpent Isle',
            engine: 'exult',
            requiredFiles: [
                'static/mainshp.flx',
                'static/shapes.vga',
                'static/u7map',
                'static/usecode',
                'static/sispeech.spc'
            ],
            optionalFiles: []
        },
        u8: {
            name: 'Ultima VIII: Pagan',
            engine: 'pentagram',
            requiredFiles: [
                'static/u8shapes.flx',
                'static/u8gumps.flx',
                'static/u8fonts.flx',
                'usecode/eusecode.flx'
            ],
            optionalFiles: [
                'static/u8sfx.flx',
                'static/u8music.flx'
            ]
        },
        u4: {
            name: 'Ultima IV: Quest of the Avatar',
            engine: 'dos',
            requiredFiles: [
                'ULTIMA4.COM',
                'PARTY.SAV'
            ],
            optionalFiles: []
        }
    };

    let db = null;

    /**
     * Initialize IndexedDB
     */
    async function init() {
        return new Promise((resolve, reject) => {
            const request = indexedDB.open(DB_NAME, DB_VERSION);

            request.onerror = () => reject(request.error);

            request.onsuccess = () => {
                db = request.result;
                console.log('[DataManager] Database initialized');
                resolve();
            };

            request.onupgradeneeded = (event) => {
                const database = event.target.result;

                // Game data store
                if (!database.objectStoreNames.contains(STORES.gameData)) {
                    const gameStore = database.createObjectStore(STORES.gameData, {
                        keyPath: 'id'
                    });
                    gameStore.createIndex('gameId', 'gameId', { unique: false });
                    gameStore.createIndex('filename', 'filename', { unique: false });
                }

                // Config store
                if (!database.objectStoreNames.contains(STORES.config)) {
                    database.createObjectStore(STORES.config, { keyPath: 'key' });
                }
            };
        });
    }

    /**
     * Store a file in IndexedDB
     * @param {string} gameId - Game identifier (u7bg, u7si, u8, u4)
     * @param {File} file - File object to store
     */
    async function storeFile(gameId, file) {
        if (!db) await init();

        const arrayBuffer = await file.arrayBuffer();
        const id = `${gameId}:${file.name}`;

        return new Promise((resolve, reject) => {
            const tx = db.transaction(STORES.gameData, 'readwrite');
            const store = tx.objectStore(STORES.gameData);

            const data = {
                id,
                gameId,
                filename: file.name,
                path: file.webkitRelativePath || file.name,
                size: file.size,
                type: file.type,
                data: arrayBuffer,
                timestamp: Date.now()
            };

            const request = store.put(data);
            request.onsuccess = () => resolve(data);
            request.onerror = () => reject(request.error);
        });
    }

    /**
     * Store multiple files for a game
     * @param {string} gameId - Game identifier
     * @param {FileList|File[]} files - Files to store
     * @param {Function} onProgress - Progress callback
     */
    async function storeFiles(gameId, files, onProgress = null) {
        const total = files.length;
        const results = [];

        for (let i = 0; i < total; i++) {
            const file = files[i];
            const result = await storeFile(gameId, file);
            results.push(result);

            if (onProgress) {
                onProgress({
                    current: i + 1,
                    total,
                    filename: file.name,
                    percent: Math.round(((i + 1) / total) * 100)
                });
            }
        }

        return results;
    }

    /**
     * Get a specific file for a game
     * @param {string} gameId - Game identifier
     * @param {string} filename - File name to retrieve
     */
    async function getFile(gameId, filename) {
        if (!db) await init();

        return new Promise((resolve, reject) => {
            const tx = db.transaction(STORES.gameData, 'readonly');
            const store = tx.objectStore(STORES.gameData);
            const id = `${gameId}:${filename}`;

            const request = store.get(id);
            request.onsuccess = () => resolve(request.result);
            request.onerror = () => reject(request.error);
        });
    }

    /**
     * Get all files for a game
     * @param {string} gameId - Game identifier
     */
    async function getGameFiles(gameId) {
        if (!db) await init();

        return new Promise((resolve, reject) => {
            const tx = db.transaction(STORES.gameData, 'readonly');
            const store = tx.objectStore(STORES.gameData);
            const index = store.index('gameId');

            const request = index.getAll(gameId);
            request.onsuccess = () => resolve(request.result);
            request.onerror = () => reject(request.error);
        });
    }

    /**
     * Check if a game has all required files
     * @param {string} gameId - Game identifier
     */
    async function validateGame(gameId) {
        const definition = GAME_DEFINITIONS[gameId];
        if (!definition) {
            return { valid: false, error: 'Unknown game' };
        }

        const files = await getGameFiles(gameId);
        const fileNames = new Set(files.map(f => f.path.toLowerCase()));

        const missing = [];
        for (const required of definition.requiredFiles) {
            const found = [...fileNames].some(f =>
                f.endsWith(required.toLowerCase()) ||
                f.includes(required.toLowerCase())
            );
            if (!found) {
                missing.push(required);
            }
        }

        return {
            valid: missing.length === 0,
            missing,
            found: files.length,
            required: definition.requiredFiles.length,
            game: definition.name
        };
    }

    /**
     * Get status of all games
     */
    async function getGamesStatus() {
        const status = {};

        for (const gameId of Object.keys(GAME_DEFINITIONS)) {
            status[gameId] = await validateGame(gameId);
        }

        return status;
    }

    /**
     * Delete all files for a game
     * @param {string} gameId - Game identifier
     */
    async function deleteGameFiles(gameId) {
        if (!db) await init();

        const files = await getGameFiles(gameId);

        return new Promise((resolve, reject) => {
            const tx = db.transaction(STORES.gameData, 'readwrite');
            const store = tx.objectStore(STORES.gameData);

            let deleted = 0;
            for (const file of files) {
                const request = store.delete(file.id);
                request.onsuccess = () => {
                    deleted++;
                    if (deleted === files.length) {
                        resolve(deleted);
                    }
                };
                request.onerror = () => reject(request.error);
            }

            if (files.length === 0) {
                resolve(0);
            }
        });
    }

    /**
     * Get storage usage information
     */
    async function getStorageInfo() {
        if (navigator.storage && navigator.storage.estimate) {
            const estimate = await navigator.storage.estimate();
            return {
                used: estimate.usage,
                quota: estimate.quota,
                percent: Math.round((estimate.usage / estimate.quota) * 100)
            };
        }
        return null;
    }

    /**
     * Save configuration value
     * @param {string} key - Config key
     * @param {*} value - Config value
     */
    async function setConfig(key, value) {
        if (!db) await init();

        return new Promise((resolve, reject) => {
            const tx = db.transaction(STORES.config, 'readwrite');
            const store = tx.objectStore(STORES.config);

            const request = store.put({ key, value });
            request.onsuccess = () => resolve();
            request.onerror = () => reject(request.error);
        });
    }

    /**
     * Get configuration value
     * @param {string} key - Config key
     */
    async function getConfig(key) {
        if (!db) await init();

        return new Promise((resolve, reject) => {
            const tx = db.transaction(STORES.config, 'readonly');
            const store = tx.objectStore(STORES.config);

            const request = store.get(key);
            request.onsuccess = () => resolve(request.result?.value);
            request.onerror = () => reject(request.error);
        });
    }

    /**
     * Export game files as a zip archive
     * @param {string} gameId - Game identifier
     */
    async function exportGameFiles(gameId) {
        // This would use a library like JSZip
        // For now, return file list
        const files = await getGameFiles(gameId);
        return files.map(f => ({
            name: f.filename,
            path: f.path,
            size: f.size
        }));
    }

    // Public API
    return {
        init,
        storeFile,
        storeFiles,
        getFile,
        getGameFiles,
        validateGame,
        getGamesStatus,
        deleteGameFiles,
        getStorageInfo,
        setConfig,
        getConfig,
        exportGameFiles,
        GAME_DEFINITIONS
    };
})();

// Export for module systems
if (typeof module !== 'undefined' && module.exports) {
    module.exports = DataManager;
}
