/**
 * CheerpX Engine Integration Module
 * Provides x86 virtualization in the browser via WebAssembly
 */

const CheerpXEngine = (function() {
    'use strict';

    // Configuration
    const CHEERPX_CDN = 'https://cxrtnc.leaningtech.com/1.0.6/cx.js';
    const FREEDOS_ASSETS = {
        bios: 'assets/bios.bin',
        vgabios: 'assets/vgabios-stdvga.bin',
        freedos: 'assets/freedos.img'
    };

    // Linux engine disk image
    const LINUX_ASSETS = {
        diskImage: 'assets/ultima-engines.ext2',
        engines: {
            exult: {
                name: 'Exult',
                binary: '/usr/bin/exult',
                launcher: '/usr/bin/run-exult',
                game: 'Ultima VII'
            },
            pentagram: {
                name: 'Pentagram',
                binary: '/usr/bin/pentagram',
                launcher: '/usr/bin/run-pentagram',
                game: 'Ultima VIII'
            }
        }
    };

    // State
    let cxInstance = null;
    let isLoaded = false;
    let loadPromise = null;

    /**
     * Load CheerpX runtime from CDN
     */
    async function loadRuntime() {
        if (loadPromise) return loadPromise;
        if (isLoaded) return Promise.resolve();

        loadPromise = new Promise((resolve, reject) => {
            const script = document.createElement('script');
            script.src = CHEERPX_CDN;
            script.async = true;

            script.onload = () => {
                isLoaded = true;
                console.log('[CheerpX] Runtime loaded successfully');
                resolve();
            };

            script.onerror = () => {
                reject(new Error('Failed to load CheerpX runtime'));
            };

            document.head.appendChild(script);
        });

        return loadPromise;
    }

    /**
     * Initialize CheerpX Linux environment
     * @param {Object} options - Configuration options
     * @returns {Promise<Object>} CheerpX instance
     */
    async function initLinux(options = {}) {
        await loadRuntime();

        if (typeof CheerpX === 'undefined') {
            throw new Error('CheerpX not available');
        }

        const defaults = {
            mounts: [],
            networkInterface: { authKey: '', controlUrl: '' }
        };

        const config = { ...defaults, ...options };

        try {
            cxInstance = await CheerpX.Linux.create(config);
            console.log('[CheerpX] Linux environment initialized');
            return cxInstance;
        } catch (error) {
            console.error('[CheerpX] Initialization failed:', error);
            throw error;
        }
    }

    /**
     * Create an ext2 filesystem from files
     * @param {FileList|File[]} files - Files to include
     * @returns {Promise<Blob>} Ext2 filesystem blob
     */
    async function createFilesystem(files) {
        // In practice, this would need server-side processing
        // or a WebAssembly-based ext2 creator
        // For now, we return the files as a virtual overlay
        const fileMap = new Map();

        for (const file of files) {
            const buffer = await file.arrayBuffer();
            fileMap.set(file.name, new Uint8Array(buffer));
        }

        return fileMap;
    }

    /**
     * Mount game data files to the virtual filesystem
     * @param {Map} fileMap - Map of filename to Uint8Array
     * @param {string} mountPoint - Mount path (e.g., '/game')
     */
    async function mountGameData(fileMap, mountPoint = '/game') {
        if (!cxInstance) {
            throw new Error('CheerpX not initialized');
        }

        // Create overlay filesystem with game data
        const overlay = await CheerpX.OverlayDevice.create(
            await CheerpX.IDBDevice.create('game-data'),
            await CheerpX.HttpBytesDevice.create(LINUX_ASSETS.diskImage)
        );

        // Mount the overlay
        await cxInstance.mount(mountPoint, overlay);

        console.log(`[CheerpX] Game data mounted at ${mountPoint}`);
    }

    /**
     * Get engine configuration
     * @param {string} engineId - Engine identifier ('exult' or 'pentagram')
     * @returns {Object} Engine configuration
     */
    function getEngineConfig(engineId) {
        return LINUX_ASSETS.engines[engineId] || null;
    }

    /**
     * Launch a game engine
     * @param {string} engineId - Engine identifier ('exult' or 'pentagram')
     * @param {string[]} args - Additional command line arguments
     * @param {Object} env - Additional environment variables
     */
    async function launchEngine(engineId, args = [], env = {}) {
        const engine = getEngineConfig(engineId);
        if (!engine) {
            throw new Error(`Unknown engine: ${engineId}`);
        }

        console.log(`[CheerpX] Launching ${engine.name} (${engine.game})`);
        return run(engine.launcher, args, env);
    }

    /**
     * Run a binary in the CheerpX environment
     * @param {string} path - Path to executable
     * @param {string[]} args - Command line arguments
     * @param {Object} env - Environment variables
     */
    async function run(path, args = [], env = {}) {
        if (!cxInstance) {
            throw new Error('CheerpX not initialized');
        }

        const defaultEnv = {
            HOME: '/root',
            PATH: '/usr/bin:/bin',
            DISPLAY: ':0',
            SDL_VIDEODRIVER: 'x11',
            ...env
        };

        console.log(`[CheerpX] Running: ${path} ${args.join(' ')}`);

        return cxInstance.run(path, args, {
            env: Object.entries(defaultEnv).map(([k, v]) => `${k}=${v}`),
            cwd: '/game'
        });
    }

    /**
     * Attach display canvas for graphics output
     * @param {HTMLCanvasElement} canvas - Target canvas element
     */
    function attachDisplay(canvas) {
        if (!cxInstance) {
            throw new Error('CheerpX not initialized');
        }

        // Set up display output
        cxInstance.setDisplaySize(canvas.width, canvas.height);
        cxInstance.setDisplayCanvas(canvas);

        console.log('[CheerpX] Display attached');
    }

    /**
     * Set up keyboard and mouse input
     * @param {HTMLElement} target - Element to capture input from
     */
    function attachInput(target) {
        if (!cxInstance) {
            throw new Error('CheerpX not initialized');
        }

        // Keyboard events
        target.addEventListener('keydown', (e) => {
            if (cxInstance.onKeyDown) {
                cxInstance.onKeyDown(e.keyCode);
                e.preventDefault();
            }
        });

        target.addEventListener('keyup', (e) => {
            if (cxInstance.onKeyUp) {
                cxInstance.onKeyUp(e.keyCode);
                e.preventDefault();
            }
        });

        // Mouse events
        target.addEventListener('mousemove', (e) => {
            if (cxInstance.onMouseMove) {
                const rect = target.getBoundingClientRect();
                cxInstance.onMouseMove(
                    e.clientX - rect.left,
                    e.clientY - rect.top
                );
            }
        });

        target.addEventListener('mousedown', (e) => {
            if (cxInstance.onMouseButton) {
                cxInstance.onMouseButton(e.button, true);
            }
        });

        target.addEventListener('mouseup', (e) => {
            if (cxInstance.onMouseButton) {
                cxInstance.onMouseButton(e.button, false);
            }
        });

        console.log('[CheerpX] Input attached');
    }

    /**
     * Clean up and destroy the CheerpX instance
     */
    function destroy() {
        if (cxInstance) {
            // Clean up resources
            cxInstance = null;
            console.log('[CheerpX] Instance destroyed');
        }
    }

    /**
     * Check if CheerpX is supported in current browser
     */
    function isSupported() {
        return typeof WebAssembly !== 'undefined' &&
               typeof SharedArrayBuffer !== 'undefined';
    }

    // Public API
    return {
        loadRuntime,
        initLinux,
        createFilesystem,
        mountGameData,
        run,
        attachDisplay,
        attachInput,
        destroy,
        isSupported,
        getInstance: () => cxInstance,
        isReady: () => cxInstance !== null,
        // New engine-specific functions
        getEngineConfig,
        launchEngine,
        getAvailableEngines: () => Object.keys(LINUX_ASSETS.engines),
        getDiskImagePath: () => LINUX_ASSETS.diskImage
    };
})();

// Export for module systems
if (typeof module !== 'undefined' && module.exports) {
    module.exports = CheerpXEngine;
}
